// 文件名: Worldclass.h
#pragma once

#include <directxmath.h>
using namespace DirectX;

// 类名: WorldClass（未完全实现）
class WorldClass
{
public:
	WorldClass();
	WorldClass(const WorldClass&);
	~WorldClass();

	void SetPosition(float, float, float);
	void SetRotation(float, float, float);
	void SetScaling(float, float, float);

	XMFLOAT3 GetPosition();
	XMFLOAT3 GetRotation();
	XMFLOAT3 GetScaling();

	void Render();
	void GetWorldMatrix(XMMATRIX&);

private:
	float m_positionX, m_positionY, m_positionZ;
	float m_rotationX, m_rotationY, m_rotationZ;
	float m_scalingX, m_scalingY, m_scalingZ;
	XMMATRIX m_worldMatrix;
};
