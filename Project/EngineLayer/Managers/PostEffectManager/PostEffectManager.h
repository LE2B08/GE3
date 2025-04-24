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

public: /// ---------- メンバ関数 ---------- ///

	// シングルトンインスタンス
	static PostEffectManager* GetInstance();

	// 初期化処理
	void Initialieze(DirectXCommon* dxCommon);

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

private: /// ---------- メンバ変数 ---------- ///

	DirectXCommon* dxCommon_ = nullptr;

	// レンダーテクスチャのクリアカラー
	const Vector4 kRenderTextureClearColor_ = { 0.08f, 0.08f, 0.18f, 1.0f }; // 分かりやすいように一旦赤色にする

	ComPtr <ID3DBlob> signatureBlob_;
	ComPtr <ID3DBlob> errorBlob_;

	ComPtr<ID3D12Resource> renderResource_;
	ComPtr<ID3D12Resource> depthResource_;

	ComPtr<ID3D12Resource> vignetteResource_;

	// 🔹 スムージングの設定
	SmoothingSetting* smoothingSetting_{};
	ComPtr<ID3D12Resource> smoothingResource_;

	// ガウシアンフィルタの設定
	GaussianFilterSetting* gaussianFilterSetting_{};
	ComPtr<ID3D12Resource> gaussianResource_;

	// アウトラインの設定
	LuminanceOutlineSetting* luminanceOutlineSetting_{};
	ComPtr<ID3D12Resource> luminanceOutlineResource_;

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

private: /// ---------- メンバ変数 ---------- ///

	// ヴィネッティングの設定
	VignetteSetting vignetteSetting_{};

	//bool enableNormalEffect = true; // 通常エフェクト
	bool enableGrayScaleEffect = false; // グレースケールエフェクト
	bool enableVignetteEffect = false; // ヴィグネットエフェクト
	bool enableSmoothingEffect = false; // スムージングエフェクト
	bool enableGaussianFilterEffect = false; // ガウシアンフィルタエフェクト
	bool enableLuminanceOutline = false; // アウトラインエフェクト

private: /// ---------- コピー禁止 ---------- ///

	PostEffectManager() = default;
	~PostEffectManager() = default;
	PostEffectManager(const PostEffectManager&) = delete;
	const PostEffectManager& operator=(const PostEffectManager&) = delete;
};
