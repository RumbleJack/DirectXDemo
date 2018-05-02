////////////////////////////////////////////////////////////////////////////////
// 文件名: textureclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "textureclass.h"

TextureClass::TextureClass()
{
	m_targaData = nullptr;
	m_texture = nullptr;
	m_textureView = nullptr;
}

TextureClass::TextureClass(const TextureClass& other)
{
}

TextureClass::~TextureClass()
{
}

bool TextureClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* filename)
{
	bool							result;
	int								height, width;
	D3D11_TEXTURE2D_DESC			textureDesc;
	HRESULT							hResult;
	unsigned int					rowPitch;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;

	// 将targa图像数据载入到内存
	result = LoadTarga(filename, height, width);
	if (!result)
		return false;

	// 建立纹理的描述
	// Setup the description of the texture.
	textureDesc.Height = height;
	textureDesc.Width = width;
	textureDesc.MipLevels = 0;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	// 与D3D11_USAGE_DYNAMIC相对的，这个宏表示资源不常被更新，而DYNAMIC表示资源是暂时的，可能经常被替换
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

	// 创建空纹理
	hResult = device->CreateTexture2D(&textureDesc, NULL, &m_texture);
	if (FAILED(hResult))
		return false;

	// 设置targa图像文件的行间隔
	rowPitch = (width * 4) * sizeof(unsigned char);

	// 赋值targa图像数据到纹理
	deviceContext->UpdateSubresource(m_texture, 0, NULL, m_targaData, rowPitch, 0);

	// 建立着色器资源试图描述
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;

	// 为纹理创建着色器资源视图
	hResult = device->CreateShaderResourceView(m_texture, &srvDesc, &m_textureView);
	if (FAILED(hResult))
		return false;

	// 为纹理生成 mipmaps 
	deviceContext->GenerateMips(m_textureView);

	// 释放targa图像数据
	delete[] m_targaData;
	m_targaData = nullptr;

	return true;
}

void TextureClass::Shutdown()
{
	// Release the texture view resource.
	if (m_textureView)
	{
		m_textureView->Release();
		m_textureView = nullptr;
	}

	// Release the texture.
	if (m_texture)
	{
		m_texture->Release();
		m_texture = nullptr;
	}

	// Release the targa data.
	if (m_targaData)
	{
		delete[] m_targaData;
		m_targaData = nullptr;
	}

	return;
}

ID3D11ShaderResourceView* TextureClass::GetTexture()
{
	return m_textureView;
}

bool TextureClass::LoadTarga(char* filename, int& height, int& width)
{
	int				error, bpp, imageSize;
	FILE*			filePtr;
	unsigned int	count;
	TargaHeader		targaFileHeader;
	unsigned char*	targaImage;

	// 以二进制方式读取targa文件
	error = fopen_s(&filePtr, filename, "rb");
	if (error != 0)
		return false;

	// 读文件头
	count = (unsigned int)fread(&targaFileHeader, sizeof(TargaHeader), 1, filePtr);
	if (count != 1)
		return false;

	// 获取文件头中信息
	height = (int)targaFileHeader.height;
	width = (int)targaFileHeader.width;
	bpp = (int)targaFileHeader.bpp;

	// 检查每个像素位数是否为32
	if (bpp != 32)
		return false;

	// 计算图像尺寸
	imageSize = width * height * 4;

	// 为图像数据分配内存
	targaImage = new unsigned char[imageSize];
	if (!targaImage)
		return false;

	// 读图像数据
	count = (unsigned int)fread(targaImage, 1, imageSize, filePtr);
	if (count != imageSize)
		return false;

	// 关闭文件
	error = fclose(filePtr);
	if (error != 0)
		return false;

	// 为图像数据分配内存
	m_targaData = new unsigned char[imageSize];
	if (!m_targaData)
		return false;

	// 初始化目标数据索引
	int index = 0;

	// 初始化原数据索引。指向实际图像的第一个像素（即二维数组的最后一行）
	int k = (width * height * 4) - (width * 4);

	// 翻转图像数据
	for (int j = 0; j < height; j++)
	{
		for (int i = 0; i < width; i++, k += 4, index += 4)
		{
			m_targaData[index + 0] = targaImage[k + 2];  // Red.
			m_targaData[index + 1] = targaImage[k + 1];  // Green.
			m_targaData[index + 2] = targaImage[k + 0];  // Blue
			m_targaData[index + 3] = targaImage[k + 3];  // Alpha
		}

		// 将原图指针指向上一行
		k -= (width * 8);
	}

	// 释放图像数据
	delete[] targaImage;
	targaImage = nullptr;

	return true;
}