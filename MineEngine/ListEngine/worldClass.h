// 文件名: Worldclass.h
#pragma once

// 类名: WorldClass（未完全实现）
class DllExport WorldClass
{
public:
	WorldClass();
	WorldClass(const WorldClass&);
	~WorldClass();

	void SetPosition(int x, int y, int z);
	void SetRotation(int x, int y, int z);
	void SetScaling (int x, int y, int z);
	void SetPosition(float, float, float);
	void SetRotation(float, float, float);
	void SetScaling(float, float, float);
	XMFLOAT3 GetPosition();
	XMFLOAT3 GetRotation();
	XMFLOAT3 GetScaling();

	// 根据当前平移、旋转、缩放计算得到世界矩阵
	void Render();
	void GetWorldMatrix(XMMATRIX&);
	void GetDefaultWorldMatrix(XMMATRIX&);

private:
	// 对象坐标系相对世界坐标系的位置、旋转、缩放
	float m_positionX, m_positionY, m_positionZ;
	float m_rotationX, m_rotationY, m_rotationZ;
	float m_scalingX,  m_scalingY,  m_scalingZ;
	// 由对象坐标系转化到世界坐标系的矩阵
	XMMATRIX m_worldMatrix;
};
