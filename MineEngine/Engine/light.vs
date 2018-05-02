////////////////////////////////////////////////////////////////////////////////
// 文件名: light.vs
////////////////////////////////////////////////////////////////////////////////


//////////////
// 全局变量 //
//////////////
cbuffer MatrixBuffer
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};
cbuffer CameraBuffer
{
    float3 cameraPosition;
    float padding;
};

//////////////
// 类型定义 //
//////////////
struct VertexInputType
{
    float4 position : POSITION;
    float2 tex :	TEXCOORD0;
    float3 normal : NORMAL;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 viewDirection : TEXCOORD1;
};


////////////////////////////////////////////////////////////////////////////////
// 顶点着色器
////////////////////////////////////////////////////////////////////////////////
PixelInputType LightVertexShader(VertexInputType input)
{
    PixelInputType output;
    
	float4 worldPosition;

    // 定义顶点的W分量为1.0f，满足齐次坐标系中的顶点定义
    input.position.w = 1.0f;

    // 通过 world, view, and projection matrices 计算顶点位置（坐标）
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
    // 为像素着色器存储纹理坐标
    output.tex = input.tex;

	// 计算经worldmatrix转化后点的法向量
    output.normal = mul(input.normal, (float3x3)worldMatrix);
	
    // 正则化法向量
    output.normal = normalize(output.normal);

	  // Calculate the position of the vertex in the world.
    worldPosition = mul(input.position, worldMatrix);

    // Determine the viewing direction based on the position of the camera and the position of the vertex in the world.
    output.viewDirection = cameraPosition.xyz - worldPosition.xyz;
	
    // Normalize the viewing direction vector.
    output.viewDirection = normalize(output.viewDirection);

    return output;
}