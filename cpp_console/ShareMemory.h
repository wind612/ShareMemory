#pragma once
#include <windows.h>
#include <string>

/*
���м����Ƚ���Ҫ�ĺ����������£�

	HANDLE CreateFileMapping(
	  HANDLE hFile,                       //�����ļ����
	  LPSECURITY_ATTRIBUTES lpAttributes, //��ȫ����
	  DWORD flProtect,                    //��������
	  DWORD dwMaximumSizeHigh,            //��λ�ļ���С
	  DWORD dwMaximumSizeLow,             //��λ�ļ���С
	  LPCTSTR lpName                      //�����ڴ�����
	);
*/

const UINT64 map_max_size = 1024000;

#define HIDWORD(a) ((DWORD)((UINT64)(a) >> 32))
#define LODWORD(a) ((DWORD)((UINT64)(a) & 0x0000ffff))

using namespace std;

class CShareMemory
{
private:
	wstring m_ShareMemoryName_write;                // �ڴ�ӳ���������
	wstring m_strMutex;
	HANDLE m_hMap_write;
	HANDLE m_hMutex;

public:
	CShareMemory();
	~CShareMemory();

	void write(BYTE* data, UINT32 data_len);
	UINT32 read(BYTE* data, UINT32 data_len);

	void test();
};

