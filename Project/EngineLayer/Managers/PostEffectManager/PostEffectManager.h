#pragma once
#include "DX12Include.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"

#include <string>
#include <numbers>
#include <unordered_map>


/// ---------- 前方宣言 ---------- ///
class DirectXCommon;


/// -------------------------------------------------------------
///				　ポストエフェクトを管理するクラス
/// -------------------------------------------------------------
class PostEffectManager
{
private: /// ---------- 構造体 ---------- ///

	// ポストエフェクトの設定
	struct PostEffectSetting
	{
		std::string effectName; // エフェクト名
		std::string shaderName; // シェーダ名
	};

	// ヴィグネットの設定
	struct VignetteSetting
	{
		float power = 0.0f; // パワー
		float range = 0.0f; // 範囲
	};

	// 🔹 スムージングの設定
	struct SmoothingSetting
	{
		float intensity; // 強度
		float threshold; // 閾値
		float sigma; // ガウス分布の標準偏差
	};

	// ガウシアンフィルタの設定
	struct GaussianFilterSetting
	{
		float intensity; // 強度
		float threshold; // 閾値
		float sigma; // ガウス関数の標準偏差
	};

	// アウトラインの設定
	struct LuminanceOutlineSetting
	{
		Vector2 texelSize;
		float edgeStrength;
		float threshold;
	};

	// ラジアルブラーの設定
	struct RadialBlurSetting
	{
		Vector2 center = { 0.5f, 0.5f };
		float blurStrength = 1.0f;
		float sampleCount = 16.0f;
	};

	struct DissolveSetting
	{
		float threshold = 0.5f; // 閾値
		float edgeThickness = 0.05f; // エッジの太さ
		Vector4 edgeColor = { 1.0f, 1.0f, 1.0f, 1.0f }; // エッジの色
		float padding[3];
	};

	struct RandomSetting
	{
		float time = 0.0f;
		bool useMultiply = false; // 乗算を使用するかどうか
		float padding[3]; // 16バイトアライメントを守る
	};

	struct AbsorbSetting
	{
		float time;
		float strength;
		float padding[2]; // アライメント調整
	};

public: /// ---------- メンバ関数 ---------- ///

	// シングルトンインスタンス
	static PostEffectManager* GetInstance();

	// 初期化処理
	void Initialieze(DirectXCommon* dxCommon);

	// 更新処理
	void Update(float deltaTime);

	// 描画開始処理
	void BeginDraw();

	// 描画終了処理
	void EndDraw();

	// ポストエフェクトの描画適用処理
	void RenderPostEffect();

	// バリアの設定
	void SetBarrier(D3D12_RESOURCE_STATES stateBefore, D3D12_RESOURCE_STATES stateAfter);

	// ImGuiの描画
	void ImGuiRender();

private: /// ---------- メンバ関数 ---------- ///

	// レンダーテクスチャリソースの生成
	ComPtr<ID3D12Resource> CreateRenderTextureResource(uint32_t width, uint32_t height, DXGI_FORMAT format, const Vector4& clearColor);

	// ルートシグネチャの生成
	void CreateRootSignature(const std::string& effectName);

	// PSOを生成
	void CreatePipelineState(const std::string& effectName);

	// ポストエフェクトを設定
	void SetPostEffect(const std::string& effectName);

private: /// ---------- メンバ関数 ---------- ///

	// RTVとSRVの確保
	void AllocateRTVAndSRV();

	// ビューポート矩形とシザリング矩形の設定
	void SetViewportAndScissorRect();

	// ヴィグネットの初期化
	void InitializeVignette();

	// スムージングの初期化
	void InitializeSmoothing();

	// ガウシアンフィルタの初期化
	void InitializeGaussianFilter();

	// アウトラインの初期化
	void InitializeLuminanceOutline();

	// ラジアルブラーの初期化
	void InitializeRadialBlur();

	// ディソルブの初期化
	void InitializeDissolve();

	// ランダムの初期化
	void InitializeRandom();

	// アブソーブの初期化
	void InitializeAbsorb();

private: /// ---------- メンバ変数 ---------- ///

	DirectXCommon* dxCommon_ = nullptr;

	// レンダーテクスチャのクリアカラー
	const Vector4 kRenderTextureClearColor_ = { 0.08f, 0.08f, 0.18f, 1.0f }; // 分かりやすいように一旦赤色にする

	ComPtr <ID3DBlob> signatureBlob_;
	ComPtr <ID3DBlob> errorBlob_;

	ComPtr<ID3D12Resource> renderResource_;
	ComPtr<ID3D12Resource> depthResource_;


	bool enableGrayScaleEffect = false; // グレースケールエフェクト

	// ヴィネッティングの設定
	VignetteSetting vignetteSetting_{};
	ComPtr<ID3D12Resource> vignetteResource_;
	bool enableVignetteEffect = false; // ヴィグネットエフェクト

	// 🔹 スムージングの設定
	SmoothingSetting* smoothingSetting_{};
	ComPtr<ID3D12Resource> smoothingResource_;
	bool enableSmoothingEffect = false; // スムージングエフェクト

	// ガウシアンフィルタの設定
	GaussianFilterSetting* gaussianFilterSetting_{};
	ComPtr<ID3D12Resource> gaussianResource_;
	bool enableGaussianFilterEffect = false; // ガウシアンフィルタエフェクト

	// アウトラインの設定
	LuminanceOutlineSetting* luminanceOutlineSetting_{};
	ComPtr<ID3D12Resource> luminanceOutlineResource_;
	bool enableLuminanceOutline = false; // アウトラインエフェクト

	// ラジアルブラーの設定
	RadialBlurSetting* radialBlurSetting_{};
	ComPtr<ID3D12Resource> radialBlurResource_;
	bool enableRadialBlur = false;

	// ディソルブの設定
	DissolveSetting* dissolveSetting_ = nullptr;
	ComPtr<ID3D12Resource> dissolveResource_;
	uint32_t dissolveMaskSrvIndex_ = 0; // SRV index for mask
	bool enableDissolveEffect = false;

	// ランダム
	RandomSetting* randomSetting_ = nullptr;
	ComPtr<ID3D12Resource> randomResource_;
	bool enableRandomEffect = false;

	// アブソーブ
	AbsorbSetting* absorbSetting_ = nullptr;
	ComPtr<ID3D12Resource> absorbResource_;
	bool enableAbsorbEffect = false;

	// ルートシグネチャ
	std::unordered_map<std::string, ComPtr<ID3D12RootSignature>> rootSignatures_;

	// パイプラインスレート
	std::unordered_map<std::string, ComPtr<ID3D12PipelineState>> graphicsPipelineStates_;

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle_;
	D3D12_CPU_DESCRIPTOR_HANDLE depthSrvHandle_;

	uint32_t rtvSrvIndex_ = 0;
	uint32_t depthSrvIndex_ = 0;

	D3D12_VIEWPORT viewport{};
	D3D12_RECT scissorRect{};

private: /// ---------- コピー禁止 ---------- ///

	PostEffectManager() = default;
	~PostEffectManager() = default;
	PostEffectManager(const PostEffectManager&) = delete;
	const PostEffectManager& operator=(const PostEffectManager&) = delete;
};
