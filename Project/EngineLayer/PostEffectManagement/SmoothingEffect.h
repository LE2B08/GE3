#pragma once
#include "IPostEffect.h"


/// -------------------------------------------------------------
///					スムージングエフェクトクラス
/// -------------------------------------------------------------
class SmoothingEffect : public IPostEffect
{
private: /// ---------- 構造体 ---------- ///

	// スムージングの設定
	struct SmoothingSetting
	{
		int kernelType = 0; // 0: none, 1: box3x3, ...
	};

public: /// ---------- メンバ関数 ---------- ///

	// 初期化処理
	void Initialize(DirectXCommon* dxCommon, PostEffectPipelineBuilder* builder) override;

	// 適用処理
	void Apply(ID3D12GraphicsCommandList* commandList, uint32_t rtvSrvIndex, uint32_t dsvSrvIndex) override;

	// ImGui描画処理
	void DrawImGui() override;

	// 名前の取得
	const std::string& GetName() const override { return name_; }

private: /// ---------- メンバ変数 ---------- ///

	// 名前
	const std::string name_ = "SmoothingEffect";

	// シェーダーコードのパス
	std::string shaderPath_ = "Resources/Shaders/PostEffect/SmoothingEffect.hlsl";

	// DirectX共通クラス
	DirectXCommon* dxCommon_ = nullptr;

	// パイプラインビルダー
	PostEffectPipelineBuilder* pipelineBuilder_;

	// グラフィックスパイプラインステートオブジェクト
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState_;

	// ルートシグネチャ
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;

	// スムージングの設定
	Microsoft::WRL::ComPtr<ID3D12Resource> constantBuffer_;
	SmoothingSetting* smoothingSetting_ = nullptr;
};

