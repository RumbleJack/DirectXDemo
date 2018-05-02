////////////////////////////////////////////////////////////////////////////////
// �ļ���: textureclass.cpp
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

	// ��targaͼ���������뵽�ڴ�
	result = LoadTarga(filename, height, width);
	if (!result)
		return false;

	// �������������
	// Setup the description of the texture.
	textureDesc.Height = height;
	textureDesc.Width = width;
	textureDesc.MipLevels = 0;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	// ��D3D11_USAGE_DYNAMIC��Եģ�������ʾ��Դ���������£���DYNAMIC��ʾ��Դ����ʱ�ģ����ܾ������滻
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

	// ����������
	hResult = device->CreateTexture2D(&textureDesc, NULL, &m_texture);
	if (FAILED(hResult))
		return false;

	// ����targaͼ���ļ����м��
	rowPitch = (width * 4) * sizeof(unsigned char);

	// ��ֵtargaͼ�����ݵ�����
	deviceContext->UpdateSubresource(m_texture, 0, NULL, m_targaData, rowPitch, 0);

	// ������ɫ����Դ��ͼ����
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;

	// Ϊ��������ɫ����Դ��ͼ
	hResult = device->CreateShaderResourceView(m_texture, &srvDesc, &m_textureView);
	if (FAILED(hResult))
		return false;

	// Ϊ�������� mipmaps 
	deviceContext->GenerateMips(m_textureView);

	// �ͷ�targaͼ������
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

	// �Զ����Ʒ�ʽ��ȡtarga�ļ�
	error = fopen_s(&filePtr, filename, "rb");
	if (error != 0)
		return false;

	// ���ļ�ͷ
	count = (unsigned int)fread(&targaFileHeader, sizeof(TargaHeader), 1, filePtr);
	if (count != 1)
		return false;

	// ��ȡ�ļ�ͷ����Ϣ
	height = (int)targaFileHeader.height;
	width = (int)targaFileHeader.width;
	bpp = (int)targaFileHeader.bpp;

	// ���ÿ������λ���Ƿ�Ϊ32
	if (bpp != 32)
		return false;

	// ����ͼ��ߴ�
	imageSize = width * height * 4;

	// Ϊͼ�����ݷ����ڴ�
	targaImage = new unsigned char[imageSize];
	if (!targaImage)
		return false;

	// ��ͼ������
	count = (unsigned int)fread(targaImage, 1, imageSize, filePtr);
	if (count != imageSize)
		return false;

	// �ر��ļ�
	error = fclose(filePtr);
	if (error != 0)
		return false;

	// Ϊͼ�����ݷ����ڴ�
	m_targaData = new unsigned char[imageSize];
	if (!m_targaData)
		return false;

	// ��ʼ��Ŀ����������
	int index = 0;

	// ��ʼ��ԭ����������ָ��ʵ��ͼ��ĵ�һ�����أ�����ά��������һ�У�
	int k = (width * height * 4) - (width * 4);

	// ��תͼ������
	for (int j = 0; j < height; j++)
	{
		for (int i = 0; i < width; i++, k += 4, index += 4)
		{
			m_targaData[index + 0] = targaImage[k + 2];  // Red.
			m_targaData[index + 1] = targaImage[k + 1];  // Green.
			m_targaData[index + 2] = targaImage[k + 0];  // Blue
			m_targaData[index + 3] = targaImage[k + 3];  // Alpha
		}

		// ��ԭͼָ��ָ����һ��
		k -= (width * 8);
	}

	// �ͷ�ͼ������
	delete[] targaImage;
	targaImage = nullptr;

	return true;
}