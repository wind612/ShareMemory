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
	wstring m_ShareMemoryName_read;                // �ڴ�ӳ���������
	wstring m_strMutex_write;
	wstring m_strMutex_read;
	HANDLE m_hMap_write;
	HANDLE m_hMap_read;
	HANDLE m_hMutex_write;
	HANDLE m_hMutex_read;

public:
	CShareMemory();
	~CShareMemory();

	void write(BYTE* data, UINT32 index, UINT32 data_len);
	void read(BYTE* data, UINT32 index, UINT32 data_len);

	void test();
};

