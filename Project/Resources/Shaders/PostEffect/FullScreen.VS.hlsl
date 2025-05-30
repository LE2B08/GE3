#include "FullScreen.hlsli"

// 頂点数
static const uint kNumVertex = 3;

// 頂点座標
static const float4 kPositions[kNumVertex] =
{
    { -1.0f, 1.0f, 0.0f, 1.0f }, // 左上
    { 3.0f, 1.0f, 0.0f, 1.0f }, // 右上
    { -1.0f, -3.0f, 0.0f, 1.0f }, // 左下
};

// テクスチャ座標
static const float2 kTexcoords[kNumVertex] =
{
    { 0.0f, 0.0f }, // 左上
    { 2.0f, 0.0f }, // 右上
    { 0.0f, 2.0f }, // 左下
};

VertexShaderOutput main(uint vertexID : SV_VertexID)
{
    VertexShaderOutput output;
    output.position = kPositions[vertexID];
    output.texcoord = kTexcoords[vertexID];
    return output;
}