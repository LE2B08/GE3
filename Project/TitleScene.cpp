#include "TitleScene.h"
#include <DirectXCommon.h>
#include <ImGuiManager.h>


/// -------------------------------------------------------------
///				　			　初期化処理
/// -------------------------------------------------------------
void TitleScene::Initialize()
{
	dxCommon_ = DirectXCommon::GetInstance();

	textureManager = TextureManager::GetInstance();

	// テクスチャのパスをリストで管理
	texturePaths_ = {
		"Resources/monsterBall.png",
		//"Resources/uvChecker.png",
	};

	/// ---------- TextureManagerの初期化 ----------///
	for (auto& texture : texturePaths_)
	{
		textureManager->LoadTexture(texture);
	}

	/// ---------- Spriteの初期化 ---------- ///
	for (uint32_t i = 0; i < 1; i++)
	{
		sprites_.push_back(std::make_unique<Sprite>());
		sprites_[i]->Initialize(texturePaths_[i % 2]);
		sprites_[i]->SetPosition(Vector2(100.0f * i, 100.0f * i));
	}

	/// ---------- サウンドの初期化 ---------- ///
	const char* fileName = "Resources/Sounds/RPGBattle01.wav";
	wavLoader_ = std::make_unique<WavLoader>();
	wavLoader_->StreamAudioAsync(fileName, 0.5f, 1.0f, false);
}


/// -------------------------------------------------------------
///				　			　 更新処理
/// -------------------------------------------------------------
void TitleScene::Update()
{
	// スプライトの更新処理
	for (auto& sprite : sprites_)
	{
		sprite->Update();
	}
}


/// -------------------------------------------------------------
///				　			　 描画処理
/// -------------------------------------------------------------
void TitleScene::Draw()
{
	/// ----- スプライトの描画設定と描画 ----- ///
	for (auto& sprite : sprites_)
	{
		sprite->Draw();
	}
}


/// -------------------------------------------------------------
///				　			　 終了処理
/// -------------------------------------------------------------

void TitleScene::Finalize()
{
	
}

void TitleScene::DrawImGui()
{
	ImGui::Begin("Test Window");

	for (uint32_t i = 0; i < sprites_.size(); i++)
	{
		ImGui::PushID(i); // スプライトごとに異なるIDを設定
		if (ImGui::TreeNode(("Sprite" + std::to_string(i)).c_str()))
		{
			Vector2 position = sprites_[i]->GetPosition();
			ImGui::DragFloat2("Position", &position.x, 1.0f);
			sprites_[i]->SetPosition(position);

			float rotation = sprites_[i]->GetRotation();
			ImGui::SliderAngle("Rotation", &rotation);
			sprites_[i]->SetRotation(rotation);

			Vector2 size = sprites_[i]->GetSize();
			ImGui::DragFloat2("Size", &size.x, 1.0f);
			sprites_[i]->SetSize(size);

			ImGui::TreePop();
		}
		ImGui::PopID(); // IDを元に戻す
	}

	ImGui::End();
}