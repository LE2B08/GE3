#pragma once
#include <vector>
#include "VertexData.h"
#include "MaterialData.h"

// ModelData構造体
struct ModelData
{
	std::vector<VertexData> vertices;
	MaterialData material;
};

