// 文件名: cameraclass.cpp
#include "cameraclass.h"

CameraClass::CameraClass()
{
	m_positionX = 0.0f;
	m_positionY = 0.0f;
	m_positionZ = 0.0f;

	m_rotationX = 0.0f;
	m_rotationY = 0.0f;
	m_rotationZ = 0.0f;


}

CameraClass::CameraClass(const CameraClass& other)
{
}

CameraClass::~CameraClass()
{
}

void CameraClass::SetPosition(float x, float y, float z)
{
	m_positionX = x;
	m_positionY = y;
	m_positionZ = z;
	return;
}

void CameraClass::SetRotation(float x, float y, float z)
{
	m_rotationX = x;
	m_rotationY = y;
	m_rotationZ = z;
	return;
}

void CameraClass::UpdePositionFromWorldToCamera(float deltaXInWorld, float deltaYInWorld, float deltaZInWorld)
{
	float yaw, pitch, roll;

	// 转化为弧度值
	pitch = m_rotationX * 0.0174532925f;
	yaw   = m_rotationY * 0.0174532925f;
	roll  = m_rotationZ * 0.0174532925f;

	// 创建旋转矩阵，使用 yaw, pitch, 和 roll 的值.
	XMMATRIX rotationMatrix;
	rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	// 创建相机位移的XMVECTOR
	XMFLOAT3 deltaPositionInWorldCordinate = XMFLOAT3(deltaXInWorld, deltaYInWorld, deltaZInWorld);
	XMVECTOR deltaPositionVectorFromWorldToCamera = XMLoadFloat3(&deltaPositionInWorldCordinate);

	// 将世界坐标系的位移，转化为相机坐标系的位移
	deltaPositionVectorFromWorldToCamera = XMVector3TransformCoord(deltaPositionVectorFromWorldToCamera, rotationMatrix);

	// 计算当前相机位置
	XMFLOAT3 deltaPositionInCameraCordinate;
	XMStoreFloat3(&deltaPositionInCameraCordinate, deltaPositionVectorFromWorldToCamera);
	m_positionX += deltaPositionInCameraCordinate.x;
	m_positionY += deltaPositionInCameraCordinate.y;
	m_positionZ += deltaPositionInCameraCordinate.z;
	
	return;
}

XMFLOAT3 CameraClass::GetPosition()
{
	return XMFLOAT3(m_positionX, m_positionY, m_positionZ);
}

XMFLOAT3 CameraClass::GetRotation()
{
	return XMFLOAT3(m_rotationX, m_rotationY, m_rotationZ);
}

void CameraClass::Render()
{
	XMFLOAT3 up, position, lookAt;

	XMVECTOR upVector, positionVector, lookAtVector;

	float yaw, pitch, roll;

	XMMATRIX rotationMatrix;

	// 建立相机上方的方向向量
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	// 建立相机在世界坐标系中的位置
	position.x = m_positionX;
	position.y = m_positionY;
	position.z = m_positionZ;

	// 建立相机观察点（默认方式）
	lookAt.x = 0.0f;
	lookAt.y = 0.0f;
	lookAt.z = 1.0f;

	// 载入 XMVECTOR 结构体
	upVector = XMLoadFloat3(&up);
	positionVector = XMLoadFloat3(&position);
	lookAtVector = XMLoadFloat3(&lookAt);

	// 转化为弧度值
	pitch = m_rotationX * 0.0174532925f;
	yaw   = m_rotationY * 0.0174532925f;
	roll  = m_rotationZ * 0.0174532925f;

	// 创建旋转矩阵，使用 yaw, pitch, 和 roll 的值.
	rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	// 通过旋转矩阵调整相机观察点和相机垂直上方（当前假设相机位置为原点）
	lookAtVector = XMVector3TransformCoord(lookAtVector, rotationMatrix);
	upVector = XMVector3TransformCoord(upVector, rotationMatrix);

	// 将旋转的相机位置移动到观察者位置（相机位置移动，则观察点位置相应移动）
	XMVECTOR lookAtPosition = XMVectorAdd(positionVector, lookAtVector);

	// 由上述三个向量建立观察者矩阵
	m_viewMatrix = XMMatrixLookAtLH(positionVector, lookAtPosition, upVector);

	return;
}

void CameraClass::GetViewMatrix(XMMATRIX& viewMatrix)
{
	viewMatrix = m_viewMatrix;
	return;
}

void CameraClass::RenderBaseViewMatrix()
{
	XMFLOAT3 up, position, lookAt;
	XMVECTOR upVector, positionVector, lookAtVector;
	float yaw, pitch, roll;
	XMMATRIX rotationMatrix;


	// Setup the vector that points upwards.
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	// Load it into a XMVECTOR structure.
	upVector = XMLoadFloat3(&up);

	// Setup the position of the camera in the world.
	position.x = m_positionX;
	position.y = m_positionY;
	position.z = m_positionZ;

	// Load it into a XMVECTOR structure.
	positionVector = XMLoadFloat3(&position);

	// Setup where the camera is looking by default.
	lookAt.x = 0.0f;
	lookAt.y = 0.0f;
	lookAt.z = 1.0f;

	// Load it into a XMVECTOR structure.
	lookAtVector = XMLoadFloat3(&lookAt);

	// Set the yaw (Y axis), pitch (X axis), and roll (Z axis) rotations in radians.
	pitch = m_rotationX * 0.0174532925f;
	yaw = m_rotationY * 0.0174532925f;
	roll = m_rotationZ * 0.0174532925f;

	// Create the rotation matrix from the yaw, pitch, and roll values.
	rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	// Transform the lookAt and up vector by the rotation matrix so the view is correctly rotated at the origin.
	lookAtVector = XMVector3TransformCoord(lookAtVector, rotationMatrix);
	upVector = XMVector3TransformCoord(upVector, rotationMatrix);

	// Translate the rotated camera position to the location of the viewer.
	lookAtVector = XMVectorAdd(positionVector, lookAtVector);

	// Finally create the view matrix from the three updated vectors.
	m_baseViewMatrix = XMMatrixLookAtLH(positionVector, lookAtVector, upVector);

	return;
}


void CameraClass::GetBaseViewMatrix(XMMATRIX& viewMatrix)
{
	viewMatrix = m_baseViewMatrix;
	return;
}