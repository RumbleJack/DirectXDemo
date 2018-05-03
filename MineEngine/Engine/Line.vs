// ������������ɫ��

cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

struct VertexInputType
{
    float4 position : POSITION;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
};

// ������ɫ��
PixelInputType TextureVertexShader(VertexInputType input)
{
    PixelInputType output;
    
	// �����붥��λ�ö���Ϊ4ά�������Խ��з���任
    input.position.w = 1.0f;

	// ������������������󡢹۲����ͶӰ�������
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
     
    return output;
}