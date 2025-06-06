#pragma once
// エフェクトの種類を列挙型で定義
enum class ParticleEffectType
{
	Default, 	// デフォルト
	Slash,		// スラッシュ
	Ring,		// リング
	Cylinder,	// シリンダー
	Star,		// 星型
	Smoke,		// 煙
	Flash,		// フラッシュ
	Spark,		// 火花
	Debris,		// 破片
	EnergyGather, //収束
	Charge,       // 新しく追加：収束中にぐるぐる回る
	Explosion     // 爆発用
};