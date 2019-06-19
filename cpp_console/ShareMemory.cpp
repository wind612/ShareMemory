#include "pch.h"
#include "ShareMemory.h"
#include <string>
#include <iostream>

//using namespace std;

CShareMemory::CShareMemory()
{
	m_ShareMemoryName_write = L"ShareMemory_to_cshap";
	m_strMutex = L"testmapmutex";

	m_hMutex = CreateMutex(NULL, FALSE, m_strMutex.c_str());
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
}


CShareMemory::~CShareMemory()
{
	// 关闭内存映射文件对象句柄
	::CloseHandle(m_hMap_write);
	::CloseHandle(m_hMutex);
}

void CShareMemory::write(BYTE* data, UINT32 data_len)
{
	try
	{
		LPVOID pBuffer;
		// 映射对象的一个视图，得到指向共享内存的指针，设置里面的数据
		pBuffer = ::MapViewOfFile(m_hMap_write, FILE_MAP_ALL_ACCESS, 0, 0, 0);
		BYTE * pData = (BYTE *)pBuffer;

		WaitForSingleObject(m_hMutex, INFINITE);
		
		// write datalen first.
		BYTE* plen = (BYTE*)&data_len;
		UINT32 i = 0;
		for (i = 0; i < sizeof(data_len); i++)
		{
			pData[i] = plen[i];
		}

		// then write data.
		for (UINT32 j = 0; j < data_len; j++)
		{
			pData[j+i] = data[j];
		}
		ReleaseMutex(m_hMutex);

		// 解除文件映射
		::UnmapViewOfFile(pBuffer);
	}
	catch (...)
	{
		cerr << "Catch a Exception." << endl;
		ReleaseMutex(m_hMutex);
	}
	
}

UINT32 CShareMemory::read(BYTE* data, UINT32 data_len)
{
	UINT32 real_len = 0;
	LPVOID pBuffer;
	// 映射对象的一个视图，得到指向共享内存的指针，设置里面的数据
	pBuffer = ::MapViewOfFile(m_hMap_write, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	BYTE * pData = (BYTE *)pBuffer;

	WaitForSingleObject(m_hMutex, INFINITE);
	BYTE* plen = (BYTE*)&real_len;
	memcpy(plen, pData, sizeof(real_len));
	std::cout << "real_len=" << dec << real_len << endl;
	if (real_len > data_len)
	{
		real_len = data_len;
	}
	memcpy(data, pData+ sizeof(real_len), real_len);
	ReleaseMutex(m_hMutex);

	// 解除文件映射
	::UnmapViewOfFile(pBuffer);

	return real_len;
}




void CShareMemory::test()
{
	wstring strMapName(L"ShareMemory_to_cshap");                // 内存映射对象名称
	string strComData("This is common data! 1");        // 共享内存中的数据
	LPVOID pBuffer;

	wstring strMutex(L"testmapmutex");
	HANDLE g_hMutex = CreateMutex(NULL, FALSE, strMutex.c_str());
	//TRUE代表主线程拥有互斥对象 但是主线程没有释放该对象  互斥对象谁拥有 谁释放  
	// FLASE代表当前没有线程拥有这个互斥对象

	// 首先试图打开一个命名的内存映射文件对象 
	HANDLE hMap = ::OpenFileMapping(FILE_MAP_ALL_ACCESS, 0, (LPCWSTR)strMapName.c_str()); // 打开共享内存
	if (NULL == hMap)
	{
		hMap = ::CreateFileMapping(INVALID_HANDLE_VALUE, //打开失败，创建一个
			NULL,
			PAGE_READWRITE,
			HIDWORD(map_max_size),           //高位文件大小
			LODWORD(map_max_size),             //低位文件大小
			(LPCWSTR)strMapName.c_str());

		//如果有问题用L"name"形式
		// 映射对象的一个视图，得到指向共享内存的指针，设置里面的数据
		pBuffer = ::MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);
		byte * pData = (byte *)pBuffer;

		WaitForSingleObject(g_hMutex, INFINITE);
		//strcpy_s((char*)pBuffer, sizeof(strComData), strComData.c_str());
		for (int i = 0; i < 10; i++)
		{
			pData[i] = (byte)i;
		}
		ReleaseMutex(g_hMutex);

		cout << "写入共享内存数据：" << (char *)pBuffer << endl;
	}
	else
	{
		cout << "共享内存已经存在，读取数据。" << endl;
		WaitForSingleObject(g_hMutex, INFINITE);

		// 打开成功，映射对象的一个视图，得到指向共享内存的指针，显示出里面的数据
		pBuffer = ::MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);
		//cout << "读取共享内存数据：" << (char *)pBuffer << endl;

		byte * pData = (byte *)pBuffer;

		//for (int i = 0; i < 5; i++)
		//{
		//	//int kk = ((byte *)pBuffer)[i];
		//	cout << dec << (int)pData[i];
		//}

		cout << "准备写数据..." << endl;
		Sleep(3000);
		for (int i = 0; i < 10; i++)
		{
			pData[i] = (byte)i;
		}
		cout << "写数据完成。" << endl;

		ReleaseMutex(g_hMutex);
	}
	getchar(); // 注意，进程关闭后，所有句柄自动关闭，所以要在这里暂停

	// 解除文件映射，关闭内存映射文件对象句柄
	::UnmapViewOfFile(pBuffer);
	::CloseHandle(hMap);
	CloseHandle(g_hMutex);
}
