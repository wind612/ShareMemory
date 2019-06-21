#pragma once
#include <windows.h>
#include <string>

/*
其中几个比较重要的函数讲解如下：

	HANDLE CreateFileMapping(
	  HANDLE hFile,                       //物理文件句柄
	  LPSECURITY_ATTRIBUTES lpAttributes, //安全设置
	  DWORD flProtect,                    //保护设置
	  DWORD dwMaximumSizeHigh,            //高位文件大小
	  DWORD dwMaximumSizeLow,             //低位文件大小
	  LPCTSTR lpName                      //共享内存名称
	);
*/

const UINT64 map_max_size = 1024000;

#define HIDWORD(a) ((DWORD)((UINT64)(a) >> 32))
#define LODWORD(a) ((DWORD)((UINT64)(a) & 0x0000ffff))

using namespace std;

class CShareMemory
{
private:
	wstring m_ShareMemoryName_write;                // 内存映射对象名称
	wstring m_ShareMemoryName_read;                // 内存映射对象名称
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

