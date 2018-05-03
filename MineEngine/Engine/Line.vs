// 绘制纹理顶点着色器

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

// 顶点着色器
PixelInputType TextureVertexShader(VertexInputType input)
{
    PixelInputType output;
    
	// 将输入顶点位置定义为4维向量，以进行仿射变换
    input.position.w = 1.0f;

	// 将顶点坐标与世界矩阵、观察矩阵及投影矩阵相乘
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
     
    return output;
}