#include "stdafx.h"
#include "HeightMapImage.h"

_3DGP_USE_
DX_USE

HeightMapImage::HeightMapImage(LPCTSTR filename, int width, int depth, const DX XMFLOAT3 & scale)
{
	m_Width = width;
	m_Depth = depth;
	m_Scale = scale;

	BYTE* pHeightMapPixels = new BYTE[m_Width * m_Depth];

	HANDLE hFile = ::CreateFile(filename, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_READONLY, NULL);
	DWORD dwBytesRead;
	ReadFile(hFile, pHeightMapPixels, (m_Width * m_Depth), &dwBytesRead, NULL);
	CloseHandle(hFile);


	m_HeightMapPixels = new BYTE[m_Width * m_Depth];
	for (int y = 0; y < m_Depth; y++)
		for (int x = 0; x < m_Width; x++)
			m_HeightMapPixels[x + ((m_Depth - 1 - y)*m_Width)] = pHeightMapPixels[x + (y*m_Width)];

	if (pHeightMapPixels) 
		delete[] pHeightMapPixels;
}

HeightMapImage::~HeightMapImage()
{
	if (m_HeightMapPixels) delete[] m_HeightMapPixels;
	m_HeightMapPixels = NULL;
}

float HeightMapImage::GetHeight(float x_, float z_) const
{
	if (!InRange(0.f, x_, m_Width) || !InRange(0.f, z_, m_Depth))
		return 0.f;

	int x = (int)x_;
	int z = (int)z_;
	float fxPercent = x_ - x;
	float fzPercent = z_ - z;

	float bottomLeft	= (float)	m_HeightMapPixels[x + (z*m_Width)];
	float bottomRight	= (float)	m_HeightMapPixels[(x + 1) + (z*m_Width)];
	float topLeft		= (float)	m_HeightMapPixels[x + ((z + 1)*m_Width)];
	float topRight		= (float)	m_HeightMapPixels[(x + 1) + ((z + 1)*m_Width)];

#ifdef _WITH_APPROXIMATE_OPPOSITE_CORNER
	bool bRightToLeft = ((z % 2) != 0);
	if (bRightToLeft)
	{
			if (fzPercent >= fxPercent)
				bottomRight = bottomLeft + (topRight - topLeft);
			else
				topLeft = topRight + (bottomLeft - bottomRight);
	}
	else
	{
			if (fzPercent < (1.0f - fxPercent))
				topRight = topLeft + (bottomRight - bottomLeft);
			else
				bottomLeft = topLeft + (bottomRight - topRight);
	}
#endif

	float topHeight = topLeft * (1 - fxPercent) + topRight * fxPercent;
	float bottomHeight = bottomLeft * (1 - fxPercent) + bottomRight * fxPercent;
	float height = bottomHeight * (1 - fzPercent) + topHeight * fzPercent;
	return height;
}

DX XMFLOAT3 HeightMapImage::GetHeightMapNormal(float x, float z) const
{
	if (!InRange(0.f, x, m_Width) || !InRange(0.f, z, m_Depth))
		return XMFLOAT3(0.f, 1.f, 0.f);

	int HeightMapIndex = x + (z * m_Width);
	int xHeightMapAdd = (x < (m_Width - 1)) ? 1 : -1;
	int zHeightMapAdd = (z < (m_Depth - 1)) ? m_Width : -m_Width;

	float y1 = (float)m_HeightMapPixels[HeightMapIndex] * m_Scale.y;
	float y2 = (float)m_HeightMapPixels[HeightMapIndex + xHeightMapAdd] * m_Scale.y;
	float y3 = (float)m_HeightMapPixels[HeightMapIndex + zHeightMapAdd] * m_Scale.y;

	XMFLOAT3 Edge1(0.f, y3 - y1, m_Scale.z);
	XMFLOAT3 Edge2(m_Scale.x, y2 - y1, 0.f);

	XMFLOAT3 Normal;
	XMStoreFloat3(&Normal, XMVector3Normalize(XMVector3Cross(XMLoadFloat3(&Edge1), XMLoadFloat3(&Edge2))));
	return Normal;
}

DX XMFLOAT3 HeightMapImage::GetScale() const
{
	return m_Scale;
}

BYTE * HeightMapImage::GetHeightMapPixels() const
{
	return m_HeightMapPixels;
}

int HeightMapImage::GetHeightMapWidth() const
{
	return m_Width;
}

int HeightMapImage::GetHeightMapDepth() const
{
	return m_Depth;
}
