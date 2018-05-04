// �ļ���: Worldclass.h
#pragma once

// ����: WorldClass��δ��ȫʵ�֣�
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

	// ���ݵ�ǰƽ�ơ���ת�����ż���õ��������
	void Render();
	void GetWorldMatrix(XMMATRIX&);
	void GetDefaultWorldMatrix(XMMATRIX&);

private:
	// ��������ϵ�����������ϵ��λ�á���ת������
	float m_positionX, m_positionY, m_positionZ;
	float m_rotationX, m_rotationY, m_rotationZ;
	float m_scalingX,  m_scalingY,  m_scalingZ;
	// �ɶ�������ϵת������������ϵ�ľ���
	XMMATRIX m_worldMatrix;
};
