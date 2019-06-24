#include "stdafx.h"
#include "HeightMapImage.h"
#include <atlimage.h>

_3DGP_USE_
DX_USE

HeightMapImage::HeightMapImage(const STD string& filename)
{
	CImage image;
	image.Load(filename.c_str());
	int PixelStride = image.GetBPP() / 8;

	m_Width = image.GetWidth();
	m_Depth = image.GetHeight();

	m_HeightMapPixels.clear();

	BITMAP bmp;
	GetObject(image, sizeof(BITMAP), &bmp);
	BITMAPINFOHEADER bmih{ 0 };
	bmih.biSize = sizeof(BITMAPINFOHEADER);
	bmih.biWidth = bmp.bmWidth;
	bmih.biHeight = bmp.bmHeight;
	bmih.biPlanes = 1;
	bmih.biBitCount = image.GetBPP();
	bmih.biCompression = (BI_RGB);

	HDC hdc = GetDC(NULL);
	GetDIBits(hdc, image, 0, bmp.bmHeight, NULL, (LPBITMAPINFO)&bmih, DIB_RGB_COLORS);
	m_HeightMapPixels.resize(bmih.biSizeImage);
	GetDIBits(hdc, image, 0, bmp.bmHeight, &(m_HeightMapPixels[0]), (LPBITMAPINFO)&bmih, DIB_RGB_COLORS);
	ReleaseDC(NULL, hdc);

	for (int y = 0; y < m_Depth; y++)
		for (int x = 0; x < m_Width; x++)
		{
			int IndexDst = x + y * m_Width;
			int IndexSrc = (x + (y*m_Width)) * PixelStride;
			m_HeightMapPixels[IndexDst] = m_HeightMapPixels[IndexSrc]; 

		}
}

HeightMapImage::~HeightMapImage()
{
}

float HeightMapImage::GetHeight(float x_, float z_) const
{
	if (!InRange(0.f, x_, (float)m_Width) || !InRange(0.f, z_, (float)m_Depth))
		return 0.f;

	int x = (int)x_;
	int z = (int)z_;
	float fxPercent = x_ - x;
	float fzPercent = z_ - z;

	float bottomLeft	= (float)	m_HeightMapPixels[x + (z*m_Width)];
	float bottomRight	= (float)	m_HeightMapPixels[(x + 1) + (z*m_Width)];
	float topLeft		= (float)	m_HeightMapPixels[x + ((z + 1)*m_Width)];
	float topRight		= (float)	m_HeightMapPixels[(x + 1) + ((z + 1)*m_Width)];

#ifndef _WITH_APPROXIMATE_OPPOSITE_CORNER
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

DX XMFLOAT3 HeightMapImage::GetHeightMapNormal(int x, int z, const DX XMFLOAT3& Scale) const
{
	if (!InRange(0, x, m_Width) || !InRange(0, z, m_Depth))
		return XMFLOAT3(0.f, 1.f, 0.f);

	int HeightMapIndex = x + (z * m_Width);
	int xHeightMapAdd = (x < (m_Width - 1)) ? 1 : -1;
	int zHeightMapAdd = (z < (m_Depth - 1)) ? m_Width : -m_Width;

	float y1 = (float)m_HeightMapPixels[HeightMapIndex] * Scale.y;
	float y2 = (float)m_HeightMapPixels[HeightMapIndex + xHeightMapAdd] * Scale.y;
	float y3 = (float)m_HeightMapPixels[HeightMapIndex + zHeightMapAdd] * Scale.y;

	XMFLOAT3 Edge1(0.f, y3 - y1, Scale.z); //Scale.z cause it might not be a 1x1x1 Scale.
	XMFLOAT3 Edge2(Scale.x, y2 - y1, 0.f); //Scale.x same.

	XMFLOAT3 Normal;
	XMStoreFloat3(&Normal, XMVector3Normalize(XMVector3Cross(XMLoadFloat3(&Edge1), XMLoadFloat3(&Edge2))));
	return Normal;
}

BYTE * HeightMapImage::GetHeightMapPixels()
{
	return m_HeightMapPixels.data();
}

int HeightMapImage::GetHeightMapWidth() const
{
	return m_Width;
}

int HeightMapImage::GetHeightMapDepth() const
{
	return m_Depth;
}
