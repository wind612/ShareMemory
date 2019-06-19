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
}


CShareMemory::~CShareMemory()
{
	// �ر��ڴ�ӳ���ļ�������
	::CloseHandle(m_hMap_write);
	::CloseHandle(m_hMutex);
}

void CShareMemory::write(BYTE* data, UINT32 data_len)
{
	try
	{
		LPVOID pBuffer;
		// ӳ������һ����ͼ���õ�ָ�����ڴ��ָ�룬�������������
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

		// ����ļ�ӳ��
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
	// ӳ������һ����ͼ���õ�ָ�����ڴ��ָ�룬�������������
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

	// ����ļ�ӳ��
	::UnmapViewOfFile(pBuffer);

	return real_len;
}




void CShareMemory::test()
{
	wstring strMapName(L"ShareMemory_to_cshap");                // �ڴ�ӳ���������
	string strComData("This is common data! 1");        // �����ڴ��е�����
	LPVOID pBuffer;

	wstring strMutex(L"testmapmutex");
	HANDLE g_hMutex = CreateMutex(NULL, FALSE, strMutex.c_str());
	//TRUE�������߳�ӵ�л������ �������߳�û���ͷŸö���  �������˭ӵ�� ˭�ͷ�  
	// FLASE����ǰû���߳�ӵ������������

	// ������ͼ��һ���������ڴ�ӳ���ļ����� 
	HANDLE hMap = ::OpenFileMapping(FILE_MAP_ALL_ACCESS, 0, (LPCWSTR)strMapName.c_str()); // �򿪹����ڴ�
	if (NULL == hMap)
	{
		hMap = ::CreateFileMapping(INVALID_HANDLE_VALUE, //��ʧ�ܣ�����һ��
			NULL,
			PAGE_READWRITE,
			HIDWORD(map_max_size),           //��λ�ļ���С
			LODWORD(map_max_size),             //��λ�ļ���С
			(LPCWSTR)strMapName.c_str());

		//�����������L"name"��ʽ
		// ӳ������һ����ͼ���õ�ָ�����ڴ��ָ�룬�������������
		pBuffer = ::MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);
		byte * pData = (byte *)pBuffer;

		WaitForSingleObject(g_hMutex, INFINITE);
		//strcpy_s((char*)pBuffer, sizeof(strComData), strComData.c_str());
		for (int i = 0; i < 10; i++)
		{
			pData[i] = (byte)i;
		}
		ReleaseMutex(g_hMutex);

		cout << "д�빲���ڴ����ݣ�" << (char *)pBuffer << endl;
	}
	else
	{
		cout << "�����ڴ��Ѿ����ڣ���ȡ���ݡ�" << endl;
		WaitForSingleObject(g_hMutex, INFINITE);

		// �򿪳ɹ���ӳ������һ����ͼ���õ�ָ�����ڴ��ָ�룬��ʾ�����������
		pBuffer = ::MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);
		//cout << "��ȡ�����ڴ����ݣ�" << (char *)pBuffer << endl;

		byte * pData = (byte *)pBuffer;

		//for (int i = 0; i < 5; i++)
		//{
		//	//int kk = ((byte *)pBuffer)[i];
		//	cout << dec << (int)pData[i];
		//}

		cout << "׼��д����..." << endl;
		Sleep(3000);
		for (int i = 0; i < 10; i++)
		{
			pData[i] = (byte)i;
		}
		cout << "д������ɡ�" << endl;

		ReleaseMutex(g_hMutex);
	}
	getchar(); // ע�⣬���̹رպ����о���Զ��رգ�����Ҫ��������ͣ

	// ����ļ�ӳ�䣬�ر��ڴ�ӳ���ļ�������
	::UnmapViewOfFile(pBuffer);
	::CloseHandle(hMap);
	CloseHandle(g_hMutex);
}
