// 文件名: cameraclass.cpp
#include "stdafx.h"
#include "cameraclass.h"

CameraClass::CameraClass()
{
	m_positionX = 0.0f;
	m_positionY = 0.0f;
	m_positionZ = -1.0f;
	
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

void CameraClass::UpdateCameraPosition(float deltaXInCamera, float deltaYInCamera, float deltaZInCamera)
{
	float yaw, pitch, roll;

	// 转化为弧度值
	pitch = m_rotationX * 0.0174532925f;
	yaw =   m_rotationY * 0.0174532925f;
	roll =  m_rotationZ * 0.0174532925f;

	// 创建旋转矩阵，使用 yaw, pitch, 和 roll 的值.
	XMMATRIX rotationMatrix;
	rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);
	
	// 创建相机位移的XMVECTOR
	XMFLOAT3 deltaPosInCamCoord = XMFLOAT3(deltaXInCamera, deltaYInCamera, deltaZInCamera);
	XMVECTOR deltaPosVecInWorldCoord = XMLoadFloat3(&deltaPosInCamCoord);

	// 将世界坐标系的位移，转化为相机坐标系的位移
	deltaPosVecInWorldCoord = XMVector3TransformCoord(deltaPosVecInWorldCoord, rotationMatrix);

	// 计算当前相机位置
	XMFLOAT3 deltaPosInWorldCoord;
	XMStoreFloat3(&deltaPosInWorldCoord, deltaPosVecInWorldCoord);
	m_positionX += deltaPosInWorldCoord.x;
	m_positionY += deltaPosInWorldCoord.y;
	m_positionZ += deltaPosInWorldCoord.z;

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

void CameraClass::GetViewMatrix(XMMATRIX & viewMatrix)
{
	MEMCPY_MATRIX(viewMatrix, m_viewMatrix);
}

void CameraClass::GetBasicViewMatrix(XMMATRIX &baseViewMatrix)
{
	SetPosition(0.0f, 0.0f, -762.0f);
	SetRotation(0.0f, 0.0f, 0.0f);
	Render();
	MEMCPY_MATRIX(baseViewMatrix, m_viewMatrix);
}

