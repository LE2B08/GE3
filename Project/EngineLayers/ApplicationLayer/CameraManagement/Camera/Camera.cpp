#include "Camera.h"
#include "MatrixMath.h"
#include "ImGuiManager.h"
#include <WinApp.h>
#include <ParameterManager.h>


Camera::Camera() :
	worldTransform({ { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f,0.0f,0.0f } }),
	fovY_(0.45f),
	aspectRatio_(float(WinApp::kClientWidth) / float(WinApp::kClientHeight)),
	nearClip_(0.1f), farClip_(100.0f),
	worldMatrix(MakeAffineMatrix(worldTransform.scale, worldTransform.rotate, worldTransform.translate)),
	viewMatrix(Inverse(worldMatrix)),
	projectionMatrix(MakePerspectiveFovMatrix(fovY_, aspectRatio_, nearClip_, farClip_)),
	viewProjevtionMatrix(Multiply(viewMatrix, projectionMatrix))
{}


/// -------------------------------------------------------------
///							更新処理
/// -------------------------------------------------------------
void Camera::Update()
{
	// ビュー行列の計算処理
	worldMatrix = MakeAffineMatrix(worldTransform.scale, worldTransform.rotate, worldTransform.translate);
	viewMatrix = Inverse(worldMatrix);

	// プロジェクション行列の更新
	projectionMatrix = MakePerspectiveFovMatrix(fovY_, aspectRatio_, nearClip_, farClip_);
	viewProjevtionMatrix = Multiply(viewMatrix, projectionMatrix);
}


/// -------------------------------------------------------------
///							ImGui描画処理
/// -------------------------------------------------------------
void Camera::DrawImGui()
{
	ImGui::Begin("Camera");
	ImGui::DragFloat3("cameraTranslate", &worldTransform.translate.x, 0.01f);
	ImGui::SliderAngle("CameraRotateX", &worldTransform.rotate.x);
	ImGui::SliderAngle("CameraRotateY", &worldTransform.rotate.y);
	ImGui::SliderAngle("CameraRotateZ", &worldTransform.rotate.z);
	ImGui::End();
}
