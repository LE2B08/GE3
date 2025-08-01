#define NOMINMAX
#include "TextureManager.h"

#include "DirectXCommon.h"
#include "SRVManager.h"
#include "ResourceManager.h"

#include <LogString.h>

#include <d3dx12.h>

#pragma comment(lib, "d3d12.lib")        // Direct3D 12用
#pragma comment(lib, "dxgi.lib")         // DXGI (DirectX Graphics Infrastructure)用
#pragma comment(lib, "dxguid.lib")       // DXGIやD3D12で使用するGUID定義用

uint32_t TextureManager::kSRVIndexTop = 1;


/// -------------------------------------------------------------
///					シングルトンインスタンス
/// -------------------------------------------------------------
TextureManager* TextureManager::GetInstance()
{
	static TextureManager instance;
	return &instance;
}

void TextureManager::Initialize(DirectXCommon* dxCommon)
{
	dxCommon_ = dxCommon;
}


/// -------------------------------------------------------------
///					リソースを作成する関数
/// -------------------------------------------------------------
ComPtr<ID3D12Resource> TextureManager::CreateTextureResource(ID3D12Device* device, const DirectX::TexMetadata& metadata)
{
	//1. metadataを基にResourceの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = UINT(metadata.width);									// Textureの幅
	resourceDesc.Height = UINT(metadata.height);								// Textureの高さ
	resourceDesc.MipLevels = UINT16(metadata.mipLevels);						// mipmapの数
	resourceDesc.DepthOrArraySize = UINT16(metadata.arraySize);					// 奥行 or 配列Textureの配列行数
	resourceDesc.Format = metadata.format;										// TextureのFormat
	resourceDesc.SampleDesc.Count = 1;											// サンプリングカウント。1固定
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension);		// Textureの次元数。普段使っているのは二次元

	//2. 利用するHeapの設定。非常に特殊な運用。02_04exで一般的なケース版がある
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_CUSTOM;								// 細かい設定を行う
	heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;		// WriteBackポリシーでCPUアクセス可能
	heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;					// プロセッサの近くに配膳

	//3. Resourceを生成する
	ComPtr <ID3D12Resource> resource = nullptr;
	HRESULT hr = device->CreateCommittedResource(
		&heapProperties,														// Heapの設定
		D3D12_HEAP_FLAG_NONE,													// Heapの特殊な設定。特になし。
		&resourceDesc,															// /Resourceの設定
		D3D12_RESOURCE_STATE_COPY_DEST,											// 初回のResourceState。Textureは基本読むだけ
		nullptr,																// Clear最適値。使わないのでnullptr
		IID_PPV_ARGS(&resource));												// 作成するResourceポインタへのポインタ
	assert(SUCCEEDED(hr));

	return resource;
}


/// -------------------------------------------------------------
///					データを転送する関数
/// -------------------------------------------------------------
[[nodiscard]]
ComPtr<ID3D12Resource> TextureManager::UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages, ID3D12Device* device, ID3D12GraphicsCommandList* commandList)
{
	std::vector<D3D12_SUBRESOURCE_DATA> subresources;
	DirectX::PrepareUpload(device, mipImages.GetImages(), mipImages.GetImageCount(), mipImages.GetMetadata(), subresources);
	uint64_t intermediateSize = GetRequiredIntermediateSize(texture, 0, UINT(subresources.size()));
	ComPtr<ID3D12Resource> intermediateResource = ResourceManager::CreateBufferResource(device, intermediateSize);
	UpdateSubresources(commandList, texture, intermediateResource.Get(), 0, 0, UINT(subresources.size()), subresources.data());

	// Textureへの転送後は利用できるよう、D3D12_RESOUCE_STATE_COPY_DESTからD3D12RESOURCE_STATE_GENERIC_READへResourceStateを変更する
	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = texture;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;

	commandList->ResourceBarrier(1, &barrier);

	return intermediateResource;
}


/// -------------------------------------------------------------
///				テクスチャデータを読み込む関数
/// -------------------------------------------------------------
DirectX::ScratchImage TextureManager::LoadTextureData(const std::string& filePath)
{
	//テクスチャファイルを呼んでプログラムで扱えるようにする
	DirectX::ScratchImage image{};
	std::wstring filePathW = ConvertString(filePath);
	HRESULT hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
	assert(SUCCEEDED(hr));

	//ミップマップの作成
	DirectX::ScratchImage mipImages{};
	hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages);
	assert(SUCCEEDED(hr));

	//ミップマップ付きのデータを返す
	return mipImages;
}


/// -------------------------------------------------------------
///			動的なテクスチャファイルを読み込む関数
/// -------------------------------------------------------------
void TextureManager::LoadTexture(const std::string& filePath)
{
	HRESULT hr{};

	// 読み込み済みテクスチャを検索
	if (textureDatas.contains(filePath)) return;

	// テクスチャファイルを読み込んでプログラムで扱えるようにする
	DirectX::ScratchImage image{};
	std::wstring filePathW = ConvertString(filePath);

	// DDSの読み込み
	if (filePathW.ends_with(L".dds"))
	{
		// .ddsで終わっていたらDDSとして読み込む
		hr = DirectX::LoadFromDDSFile(filePathW.c_str(), DirectX::DDS_FLAGS_NONE, nullptr, image);
	}
	else
	{
		// WIC形式（pngやjpgなど）として読み込む
		hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
		// 読み込みに失敗した場合はエラーを出力
		assert(SUCCEEDED(hr));
	}

	// ミップマップの作成（ScratchImageに保持する）
	DirectX::ScratchImage mipImages{};
	const auto& meta = image.GetMetadata();

	// ミップレベル数を画像サイズから自動算出
	size_t maxMips = static_cast<size_t>(std::log2(std::max(meta.width, meta.height))) + 1;
	size_t mipLevels = std::min(maxMips, size_t(4)); // 最大4ミップまで生成

	if (DirectX::IsCompressed(meta.format))
	{
		// 圧縮フォーマットならミップ生成せずそのまま使う
		mipImages = std::move(image);
	}
	else if (meta.width > 1 && meta.height > 1)
	{
		// 非圧縮かつ2x2以上ならミップマップを生成
		hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), meta, DirectX::TEX_FILTER_SRGB, mipLevels, mipImages);
		assert(SUCCEEDED(hr));
	}
	else
	{
		// 1x1などミップマップを生成できないサイズのときは、そのまま1レベルの画像として初期化
		hr = mipImages.InitializeFromImage(*image.GetImages()); // ← ここで参照渡し
		assert(SUCCEEDED(hr));
	}

	// 追加したテクスチャデータの参照を取得
	TextureData& textureData = textureDatas[filePath];

	// テクスチャリソースの生成
	textureData.metaData = mipImages.GetMetadata();
	textureData.resource = CreateTextureResource(dxCommon_->GetDevice(), textureData.metaData);

	// 中間リソースにデータを転送
	ComPtr<ID3D12Resource> intermediateResource = UploadTextureData(textureData.resource.Get(), mipImages, dxCommon_->GetDevice(), dxCommon_->GetCommandManager()->GetCommandList());

	// コマンドを実行し完了まで待機
	dxCommon_->GetCommandManager()->ExecuteAndWait();

	// SRV（Shader Resource View）の確保
	textureData.srvIndex = SRVManager::GetInstance()->Allocate();
	textureData.srvHandleCPU = SRVManager::GetInstance()->GetCPUDescriptorHandle(textureData.srvIndex);
	textureData.srvHandleGPU = SRVManager::GetInstance()->GetGPUDescriptorHandle(textureData.srvIndex);

	// SRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = textureData.metaData.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	if (textureData.metaData.IsCubemap())
	{
		// キューブマップとして扱う場合
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
		srvDesc.TextureCube.MostDetailedMip = 0;
		srvDesc.TextureCube.MipLevels = UINT_MAX;
		srvDesc.TextureCube.ResourceMinLODClamp = 0.0f;
	}
	else
	{
		// 通常の2Dテクスチャ
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = UINT(textureData.metaData.mipLevels);
	}

	// SRVの生成
	dxCommon_->GetDevice()->CreateShaderResourceView(textureData.resource.Get(), &srvDesc, textureData.srvHandleCPU);
}


/// -------------------------------------------------------------
///					テクスチャの再読み込み
/// -------------------------------------------------------------
void TextureManager::ReloadTexture(const std::string& filePath)
{
	// 既存テクスチャが存在する場合、古いリソースを解放
	auto it = textureDatas.find(filePath);
	if (it != textureDatas.end())
	{
		SRVManager::GetInstance()->Free(it->second.srvIndex);
		it->second.resource.Reset();
	}

	// 新しいテクスチャを読み込む
	LoadTexture(filePath);
}


/// -------------------------------------------------------------
///					デスクリプタテーブルの設定
/// -------------------------------------------------------------
void TextureManager::SetGraphicsRootDescriptorTable(ID3D12GraphicsCommandList* commandList, UINT rootParameter, D3D12_GPU_DESCRIPTOR_HANDLE textureSRVHandleGPU)
{
	// ディスクリプタヒープの設定
	SRVManager::GetInstance()->PreDraw();

	// ディスクリプタテーブルの設定
	commandList->SetGraphicsRootDescriptorTable(rootParameter, textureSRVHandleGPU);
}


/// -------------------------------------------------------------
///						SRVの開始番号の取得
/// -------------------------------------------------------------
uint32_t TextureManager::GetTextureIndexByFilePath(const std::string& filePath)
{
	auto it = textureDatas.find(filePath);
	if (it != textureDatas.end()) {
		return it->second.srvIndex;
	}

	throw std::runtime_error("Texture not found: " + filePath); // 例外をスロー
}


/// -------------------------------------------------------------
///						GPUハンドルの取得
/// -------------------------------------------------------------
D3D12_GPU_DESCRIPTOR_HANDLE TextureManager::GetSrvHandleGPU(const std::string& filePath)
{
	// 範囲外指定違反チェック
	assert(textureDatas.find(filePath) != textureDatas.end()); // テクスチャ番号が正常範囲内である

	// テクスチャデータの参照を取得
	TextureData& textureData = textureDatas[filePath];

	// GPUハンドルを返す
	return textureData.srvHandleGPU;
}

uint32_t TextureManager::GetSrvIndex(const std::string& filePath)
{
	// 範囲外指定違反チェック
	assert(textureDatas.find(filePath) != textureDatas.end()); // テクスチャ番号が正常範囲内である

	// テクスチャデータの参照を取得
	TextureData& textureData = textureDatas[filePath];

	// GPUハンドルを返す
	return textureData.srvIndex;
}


/// -------------------------------------------------------------
///						メタデータを取得
/// -------------------------------------------------------------
const DirectX::TexMetadata& TextureManager::GetMetaData(const std::string& filePath)
{
	// 範囲外指定違反チェック
	assert(textureDatas.find(filePath) != textureDatas.end()); // テクスチャ番号が正常範囲内である

	// テクスチャデータの参照を取得
	TextureData& textureData = textureDatas[filePath];

	// GPUハンドルを返す
	return textureData.metaData;
}

