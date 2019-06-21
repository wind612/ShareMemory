#include "pch.h"
#include "ShareMemory.h"
#include <string>
#include <iostream>

//using namespace std;

CShareMemory::CShareMemory()
{
	m_ShareMemoryName_write = L"ShareMemory_to_cshap";
	m_ShareMemoryName_read = L"ShareMemory_to_cpp";
	m_strMutex_write = L"testmapmutex_write";
	m_strMutex_read = L"testmapmutex_read";

	m_hMutex_write = CreateMutex(NULL, FALSE, m_strMutex_write.c_str());
	m_hMutex_read = CreateMutex(NULL, FALSE, m_strMutex_read.c_str());
	//TRUE代表主线程拥有互斥对象 但是主线程没有释放该对象  互斥对象谁拥有 谁释放  
	// FLASE代表当前没有线程拥有这个互斥对象

	// 首先试图打开一个命名的内存映射文件对象 
	m_hMap_write = ::OpenFileMapping(FILE_MAP_ALL_ACCESS, 0, (LPCWSTR)m_ShareMemoryName_write.c_str()); // 打开共享内存
	if (NULL == m_hMap_write)
	{
		m_hMap_write = ::CreateFileMapping(INVALID_HANDLE_VALUE, //打开失败，创建一个
			NULL,
			PAGE_READWRITE,
			HIDWORD(map_max_size),           //高位文件大小
			LODWORD(map_max_size),             //低位文件大小
			(LPCWSTR)m_ShareMemoryName_write.c_str());
	}

	m_hMap_read = ::OpenFileMapping(FILE_MAP_ALL_ACCESS, 0, (LPCWSTR)m_ShareMemoryName_read.c_str()); // 打开共享内存
	if (NULL == m_hMap_read)
	{
		m_hMap_read = ::CreateFileMapping(INVALID_HANDLE_VALUE, //打开失败，创建一个
			NULL,
			PAGE_READWRITE,
			HIDWORD(map_max_size),           //高位文件大小
			LODWORD(map_max_size),             //低位文件大小
			(LPCWSTR)m_ShareMemoryName_read.c_str());
	}
}


CShareMemory::~CShareMemory()
{
	// 关闭内存映射文件对象句柄
	::CloseHandle(m_hMap_write);
	::CloseHandle(m_hMap_read);
	::CloseHandle(m_hMutex_write);
	::CloseHandle(m_hMutex_read);
}

void CShareMemory::write(BYTE* data, UINT32 index, UINT32 data_len)
{
	try
	{
		LPVOID pBuffer;
		// 映射对象的一个视图，得到指向共享内存的指针，设置里面的数据
		pBuffer = ::MapViewOfFile(m_hMap_write, FILE_MAP_ALL_ACCESS, 0, 0, 0);
		BYTE * pData = (BYTE *)pBuffer;

		WaitForSingleObject(m_hMutex_write, INFINITE);		
		memcpy((BYTE *)pBuffer + index, data, data_len);
		ReleaseMutex(m_hMutex_write);

		// 解除文件映射
		::UnmapViewOfFile(pBuffer);
	}
	catch (...)
	{
		cerr << "Catch a Exception." << endl;
		ReleaseMutex(m_hMutex_write);
	}
	
}

void CShareMemory::read(BYTE* data, UINT32 index, UINT32 data_len)
{
	LPVOID pBuffer;

	try
	{
		// 映射对象的一个视图，得到指向共享内存的指针，设置里面的数据
		pBuffer = ::MapViewOfFile(m_hMap_read, FILE_MAP_ALL_ACCESS, 0, 0, 0);

		WaitForSingleObject(m_hMutex_read, INFINITE);
		memcpy(data, (BYTE *)pBuffer + index, data_len);
		ReleaseMutex(m_hMutex_read);

		// 解除文件映射
		::UnmapViewOfFile(pBuffer);
	}
	catch (...)
	{
		cerr << "Catch a Exception." << endl;
		ReleaseMutex(m_hMutex_read);
	}	
}
