#pragma once
#include "WinApp.h"
#include "Input.h"
#include "DirectXCommon.h"
#include "ImGuiManager.h"
#include "D3DResourceLeakChecker.h"
#include "LogString.h"
#include "PipelineStateManager.h"
#include "TextureManager.h"
#include "Sprite.h"
#include "ResourceManager.h"
#include "Object3D.h"
#include "Object3DCommon.h"
#include "ModelManager.h"
#include "WavLoader.h"
#include "SRVManager.h"

/// -------------------------------------------------------------
///				　	ゲーム全体を管理するクラス
/// -------------------------------------------------------------
class GameEngine
{
public: /// ---------- メンバ関数 ---------- ///

	// 初期化処理
	void Initialize(uint32_t Width, uint32_t Height);

	// 更新処理
	void Update();

	// 描画処理
	void Draw();

	// 終了処理
	void Finalize();

private: /// ---------- メンバ変数 ---------- ///

	WinApp* winApp;
	DirectXCommon* dxCommon;
	Input* input;
	ImGuiManager* imguiManager;
	TextureManager* textureManager;
	ModelManager* modelManager;
	SRVManager* srvManager;

	std::unique_ptr<Camera> camera_;
	std::vector<std::unique_ptr<Sprite>> sprites_;
	std::vector<std::unique_ptr<Object3D>> objects3D_;
	std::unique_ptr<Object3DCommon> object3DCommon_;
	std::unique_ptr<PipelineStateManager> pipelineStateManager_;
	std::unique_ptr<WavLoader> wavLoader_;

	// テクスチャのパスをリストで管理
	std::vector<std::string> texturePaths_;
	std::vector<std::string> objectFiles;
	std::vector<Vector3> initialPositions;


private: /// ---------- メンバ変数 ---------- ///
	SoundData soundData;
};
