// 文件名: cameraclass.h
#pragma once

class DllExport CameraClass
{
public:
	CameraClass();
	CameraClass(const CameraClass&);
	~CameraClass();

	void SetPosition(float, float, float);
	void SetRotation(float, float, float);
	XMFLOAT3 GetPosition();
	XMFLOAT3 GetRotation();

	// 将相机坐标系中的位移转化为世界坐标系中的位移，更新相机在世界坐标系中的位置
	void UpdateCameraPosition( float deltaX, float deltaY, float deltaZ);
	// 使用当前相机的位置和旋转角度计算出观察矩阵
	void Render();
	// 获取观察矩阵
	void GetViewMatrix(XMMATRIX&);
	void GetBasicViewMatrix(XMMATRIX&);
private:
	// 相机在世界坐标系中的位置及旋转角度
	float m_positionX, m_positionY, m_positionZ;
	float m_rotationX, m_rotationY, m_rotationZ;
	// 由世界坐标系转化到相机坐标系的矩阵
	XMMATRIX m_viewMatrix;
};
