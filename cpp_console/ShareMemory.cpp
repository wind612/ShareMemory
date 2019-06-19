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
	//TRUE�������߳�ӵ�л������ �������߳�û���ͷŸö���  �������˭ӵ�� ˭�ͷ�  
	// FLASE����ǰû���߳�ӵ������������

	// ������ͼ��һ���������ڴ�ӳ���ļ����� 
	m_hMap_write = ::OpenFileMapping(FILE_MAP_ALL_ACCESS, 0, (LPCWSTR)m_ShareMemoryName_write.c_str()); // �򿪹����ڴ�
	if (NULL == m_hMap_write)
	{
		m_hMap_write = ::CreateFileMapping(INVALID_HANDLE_VALUE, //��ʧ�ܣ�����һ��
			NULL,
			PAGE_READWRITE,
			HIDWORD(map_max_size),           //��λ�ļ���С
			LODWORD(map_max_size),             //��λ�ļ���С
			(LPCWSTR)m_ShareMemoryName_write.c_str());
	}

	m_hMap_read = ::OpenFileMapping(FILE_MAP_ALL_ACCESS, 0, (LPCWSTR)m_ShareMemoryName_read.c_str()); // �򿪹����ڴ�
	if (NULL == m_hMap_read)
	{
		m_hMap_read = ::CreateFileMapping(INVALID_HANDLE_VALUE, //��ʧ�ܣ�����һ��
			NULL,
			PAGE_READWRITE,
			HIDWORD(map_max_size),           //��λ�ļ���С
			LODWORD(map_max_size),             //��λ�ļ���С
			(LPCWSTR)m_ShareMemoryName_read.c_str());
	}
}


CShareMemory::~CShareMemory()
{
	// �ر��ڴ�ӳ���ļ�������
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
		// ӳ������һ����ͼ���õ�ָ�����ڴ��ָ�룬�������������
		pBuffer = ::MapViewOfFile(m_hMap_write, FILE_MAP_ALL_ACCESS, 0, 0, 0);
		BYTE * pData = (BYTE *)pBuffer;

		WaitForSingleObject(m_hMutex_write, INFINITE);		
		memcpy((BYTE *)pBuffer + index, data, data_len);
		ReleaseMutex(m_hMutex_write);

		// ����ļ�ӳ��
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
		// ӳ������һ����ͼ���õ�ָ�����ڴ��ָ�룬�������������
		pBuffer = ::MapViewOfFile(m_hMap_read, FILE_MAP_ALL_ACCESS, 0, 0, 0);

		WaitForSingleObject(m_hMutex_read, INFINITE);
		memcpy(data, (BYTE *)pBuffer + index, data_len);
		ReleaseMutex(m_hMutex_read);

		// ����ļ�ӳ��
		::UnmapViewOfFile(pBuffer);
	}
	catch (...)
	{
		cerr << "Catch a Exception." << endl;
		ReleaseMutex(m_hMutex_read);
	}	
}
