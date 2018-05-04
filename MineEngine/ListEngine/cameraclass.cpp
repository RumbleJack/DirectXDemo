// �ļ���: cameraclass.cpp
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

	// ת��Ϊ����ֵ
	pitch = m_rotationX * 0.0174532925f;
	yaw =   m_rotationY * 0.0174532925f;
	roll =  m_rotationZ * 0.0174532925f;

	// ������ת����ʹ�� yaw, pitch, �� roll ��ֵ.
	XMMATRIX rotationMatrix;
	rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);
	
	// �������λ�Ƶ�XMVECTOR
	XMFLOAT3 deltaPosInCamCoord = XMFLOAT3(deltaXInCamera, deltaYInCamera, deltaZInCamera);
	XMVECTOR deltaPosVecInWorldCoord = XMLoadFloat3(&deltaPosInCamCoord);

	// ����������ϵ��λ�ƣ�ת��Ϊ�������ϵ��λ��
	deltaPosVecInWorldCoord = XMVector3TransformCoord(deltaPosVecInWorldCoord, rotationMatrix);

	// ���㵱ǰ���λ��
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

	// ��������Ϸ��ķ�������
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	// �����������������ϵ�е�λ��
	position.x = m_positionX;
	position.y = m_positionY;
	position.z = m_positionZ;

	// ��������۲�㣨Ĭ�Ϸ�ʽ��
	lookAt.x = 0.0f;
	lookAt.y = 0.0f;
	lookAt.z = 1.0f;

	// ���� XMVECTOR �ṹ��
	upVector = XMLoadFloat3(&up);
	positionVector = XMLoadFloat3(&position);
	lookAtVector = XMLoadFloat3(&lookAt);

	// ת��Ϊ����ֵ
	pitch = m_rotationX * 0.0174532925f;
	yaw   = m_rotationY * 0.0174532925f;
	roll  = m_rotationZ * 0.0174532925f;

	// ������ת����ʹ�� yaw, pitch, �� roll ��ֵ.
	rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	// ͨ����ת�����������۲��������ֱ�Ϸ�����ǰ�������λ��Ϊԭ�㣩
	lookAtVector = XMVector3TransformCoord(lookAtVector, rotationMatrix);
	upVector = XMVector3TransformCoord(upVector, rotationMatrix);

	// ����ת�����λ���ƶ����۲���λ�ã����λ���ƶ�����۲��λ����Ӧ�ƶ���
	XMVECTOR lookAtPosition = XMVectorAdd(positionVector, lookAtVector);

	// �������������������۲��߾���
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

