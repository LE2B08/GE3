#pragma once
#include "DX12Include.h"
#include "WorldTransform.h"
#include "TextureManager.h"
#include "Material.h"
#include "Mesh.h"
#include "VertexData.h"
#include "ModelData.h"
#include "Camera.h"
#include "LightManager.h"

#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <numbers>

/// ---------- 前方宣言 ---------- ///
class DirectXCommon;
class Model;
class Object3DCommon;
class SkyBox;


/// -------------------------------------------------------------
///						オブジェクト3Dクラス
/// -------------------------------------------------------------
class Object3D
{
public: /// ---------- 構造体 ---------- ///

	// シェーダー側のカメラ構造体
	struct CameraForGPU
	{
		Vector3 worldPosition;
	};

public: /// ---------- メンバ関数 ---------- ///

	// 初期化処理
	void Initialize(const std::string& fileName);

	// 更新処理
	void Update();

	// ImGui
	void DrawImGui();

	// 描画処理
	void Draw();

public: /// ---------- 設定処理 ---------- ///

	// モデルの追加
	void SetModel(const std::string& filePath);

	// スケールを設定
	void SetScale(const Vector3& scale) { worldTransform.scale_ = scale; }

	// 回転を設定
	void SetRotate(const Vector3& rotate) { worldTransform.rotate_ = rotate; }

	// 位置を設定
	void SetTranslate(const Vector3& translate) { worldTransform.translate_ = translate; }

	// カメラの設定
	void SetCamera(Camera* camera) { camera_ = camera; }

	// 反射率の設定
	void SetReflectivity(float reflectivity) { material_.SetShininess(reflectivity); }

public: /// ---------- ゲッタ ---------- ///

private: /// ---------- メンバ変数 ---------- ///

	// カメラ用のリソース生成
	void InitializeCameraResource();

private: /// ---------- メンバ変数 ---------- ///

	DirectXCommon* dxCommon_ = nullptr;
	Camera* camera_ = nullptr;
	SkyBox* skyBox_ = nullptr;

	std::shared_ptr<Model> model_;

	// マテリアルデータ
	Material material_;

	// ワールドトランスフォーム
	WorldTransform worldTransform;

	// メッシュ
	Mesh mesh_;

	// バッファリソースの作成
	ComPtr <ID3D12Resource> cameraResource;

	// カメラにデータを書き込む
	CameraForGPU* cameraData = nullptr;

	// OBJファイルのデータ
	ModelData modelData;

	float alpha = 1.0f; // α値

	// 環境マップのテクスチャ
	D3D12_GPU_DESCRIPTOR_HANDLE environmentMapHandle_{};
};
