// �ļ���: cameraclass.h
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

	// ���������ϵ�е�λ��ת��Ϊ��������ϵ�е�λ�ƣ������������������ϵ�е�λ��
	void UpdateCameraPosition( float deltaX, float deltaY, float deltaZ);
	// ʹ�õ�ǰ�����λ�ú���ת�Ƕȼ�����۲����
	void Render();
	// ��ȡ�۲����
	void GetViewMatrix(XMMATRIX&);
	void GetBasicViewMatrix(XMMATRIX&);
private:
	// �������������ϵ�е�λ�ü���ת�Ƕ�
	float m_positionX, m_positionY, m_positionZ;
	float m_rotationX, m_rotationY, m_rotationZ;
	// ����������ϵת�����������ϵ�ľ���
	XMMATRIX m_viewMatrix;
};
