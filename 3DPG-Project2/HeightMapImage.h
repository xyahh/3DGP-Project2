#pragma once

_3DGP_BEGIN_

class HeightMapImage
{
public:
	HeightMapImage(const STD string& filename);
	~HeightMapImage();

	float GetHeight(float x, float z) const;

	DX XMFLOAT3 GetHeightMapNormal(int x, int z, const DX XMFLOAT3& Scale) const;

	BYTE* GetHeightMapPixels();

	int GetHeightMapWidth() const;
	int GetHeightMapDepth() const;

private:

	STD vector<BYTE>	m_HeightMapPixels;
	int					m_Width;
	int					m_Depth;
};

_3DGP_END_