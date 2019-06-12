#pragma once

_3DGP_BEGIN_

class HeightMapImage
{
public:
	HeightMapImage(LPCTSTR filename, int width, int depth, const DX XMFLOAT3& scale);
	~HeightMapImage();

	float GetHeight(float x, float z) const;

	DX XMFLOAT3 GetHeightMapNormal(float x, float z) const;

	DX XMFLOAT3 GetScale() const;

	BYTE* GetHeightMapPixels() const;

	int GetHeightMapWidth() const;
	int GetHeightMapDepth() const;

private:

	BYTE*		m_HeightMapPixels;
	int			m_Width;
	int			m_Depth;
	DX XMFLOAT3 m_Scale;
};

_3DGP_END_