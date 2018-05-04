// �ļ���: Worldclass.cpp
#include "stdafx.h"
#include "Worldclass.h"

WorldClass::WorldClass()
{
	m_positionX = 0.0f;
	m_positionY = 0.0f;
	m_positionZ = 0.0f;

	m_rotationX = 0.0f;
	m_rotationY = 0.0f;
	m_rotationZ = 0.0f;

	m_scalingX = 0.0f;
	m_scalingY = 0.0f;
	m_scalingZ = 0.0f;
}

WorldClass::WorldClass(const WorldClass& other)
{
}

WorldClass::~WorldClass()
{
}
void WorldClass::SetPosition(int x, int y, int z)
{
	SetPosition((float)x, (float)y, (float)z);
}
void WorldClass::SetRotation(int x, int y, int z) 
{
	SetRotation((float)x, (float)y, (float)z);
}
void WorldClass::SetScaling(int x, int y, int z)
{
	SetScaling((float)x, (float)y, (float)z);
}

void WorldClass::SetPosition(float x, float y, float z)
{
	m_positionX = x;
	m_positionY = y;
	m_positionZ = z;
	return;
}

void WorldClass::SetRotation(float x, float y, float z)
{
	m_rotationX = x;
	m_rotationY = y;
	m_rotationZ = z;
	return;
}

void WorldClass::SetScaling(float, float, float)
{
}

XMFLOAT3 WorldClass::GetPosition()
{
	return XMFLOAT3(m_positionX, m_positionY, m_positionZ);
}

XMFLOAT3 WorldClass::GetRotation()
{
	return XMFLOAT3(m_rotationX, m_rotationY, m_rotationZ);
}

XMFLOAT3 WorldClass::GetScaling()
{
	return XMFLOAT3();
}

void WorldClass::Render()
{
	XMMATRIX T1,T2;

	// ��ƽ�ơ����š���ת����World����
	T1 = XMMatrixTranslation(m_positionX, m_positionY, m_positionZ);
	T2 = XMMatrixRotationY(m_rotationY);

	m_worldMatrix = T1*T2;

	// ��ƽ�ơ����š���ת����World����
	m_worldMatrix= XMMatrixTranslation(m_positionX, m_positionY, m_positionZ);
	

	//ÿ�������������󣬽�����λ
	m_positionX = 0.0f;
	m_positionY = 0.0f;
	m_positionZ = 0.0f;

	m_rotationX = 0.0f;
	m_rotationY = 0.0f;
	m_rotationZ = 0.0f;

	m_scalingX = 0.0f;
	m_scalingY = 0.0f;
	m_scalingZ = 0.0f;

	return;
}

void WorldClass::GetWorldMatrix(XMMATRIX& worldMatrix)
{
	worldMatrix = m_worldMatrix;
	return;
}

void WorldClass::GetDefaultWorldMatrix(XMMATRIX &worldMatrix)
{
	// ��ʼ���������Ϊ��λ��
	m_worldMatrix = XMMatrixIdentity();
	worldMatrix = m_worldMatrix;
	return;
}

