#pragma once
#include <LevelData.h>
#include <memory>

/// -------------------------------------------------------------
///				　		レベルローダークラス
/// -------------------------------------------------------------
class LevelLoader
{
public: /// ---------- �����o�֐� ---------- ///

	// ���x���f�[�^�̓ǂݍ���
	static std::unique_ptr<LevelData> LoadLevel(const std::string& filePath);
	
};

