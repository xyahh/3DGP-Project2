#pragma once

_3DGP_BEGIN_

//Reference: http://nikvoss.com/2013/03/implementing-id3dinclude/

inline STD string GetWorkingDir()
{
	char path[MAX_PATH] = "";
	assert(GetCurrentDirectoryA(MAX_PATH, path));
	return STD string(path) + "\\";
}

inline STD string GetSystemDir(bool Wow64Dir = false)
{
	char max_path[MAX_PATH] = "";

	if (Wow64Dir)
		assert(GetSystemWow64DirectoryA(max_path, MAX_PATH));
	else
		assert(GetSystemDirectoryA(max_path, MAX_PATH));

	return STD string(max_path) + "\\";
}

static const STD string WORKING_DIR{ GetWorkingDir() };
static const STD string SYSTEM_DIR { GetSystemDir() };

class ShaderInclude : public ID3DInclude 
{
public:

	ShaderInclude(const STD string& shaderDir = WORKING_DIR, const STD string& systemDir = SYSTEM_DIR)
		: m_ShaderDir(shaderDir), m_SystemDir(systemDir) {}

	HRESULT __stdcall ShaderInclude::Open(D3D_INCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID * ppData, UINT * pBytes)
	{
		try 
		{
			STD string finalPath;

			switch (IncludeType)
			{
			case D3D_INCLUDE_LOCAL:
				finalPath = m_ShaderDir + pFileName;
				break;
			case D3D_INCLUDE_SYSTEM:
				finalPath = m_SystemDir + pFileName;
				break;
			default:
				assert(0);
			}

			STD ifstream includeFile(finalPath, STD ios::in | STD ios::binary | STD ios::ate);

			if (includeFile.is_open())
			{
				long long fileSize = includeFile.tellg();
				char* buf = new char[fileSize];
				includeFile.seekg(0, STD ios::beg);
				includeFile.read(buf, fileSize);
				includeFile.close();
				*ppData = buf;
				*pBytes = (UINT)fileSize;
			}
			else
			{
				return E_FAIL;
			}
			return S_OK;
		}

		catch (STD exception&) 
		{
			return E_FAIL;
		}
	}

	HRESULT __stdcall ShaderInclude::Close(LPCVOID pData)
	{
		char* buf = (char*)pData;
		delete[] buf;
		pData = NULL;
		return S_OK;
	}

private:
	STD string m_ShaderDir;
	STD string m_SystemDir;
};

_3DGP_END_