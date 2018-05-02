////////////////////////////////////////////////////////////////////////////////
// �ļ���: light.vs
////////////////////////////////////////////////////////////////////////////////


//////////////
// ȫ�ֱ��� //
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
// ���Ͷ��� //
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
// ������ɫ��
////////////////////////////////////////////////////////////////////////////////
PixelInputType LightVertexShader(VertexInputType input)
{
    PixelInputType output;
    
	float4 worldPosition;

    // ���嶥���W����Ϊ1.0f�������������ϵ�еĶ��㶨��
    input.position.w = 1.0f;

    // ͨ�� world, view, and projection matrices ���㶥��λ�ã����꣩
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
    // Ϊ������ɫ���洢��������
    output.tex = input.tex;

	// ���㾭worldmatrixת�����ķ�����
    output.normal = mul(input.normal, (float3x3)worldMatrix);
	
    // ���򻯷�����
    output.normal = normalize(output.normal);

	  // Calculate the position of the vertex in the world.
    worldPosition = mul(input.position, worldMatrix);

    // Determine the viewing direction based on the position of the camera and the position of the vertex in the world.
    output.viewDirection = cameraPosition.xyz - worldPosition.xyz;
	
    // Normalize the viewing direction vector.
    output.viewDirection = normalize(output.viewDirection);

    return output;
}