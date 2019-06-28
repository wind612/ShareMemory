#include "pch.h"
#include "NamePipeTest.h"

#include <iostream>
#include <thread>
#include <windows.h>

using namespace std;

void NamePipeWrite() 
{
	std::cout << "in NamePipeWrite.\n";
	wcout << "Creating an instance of a named pipe..." << endl;	

	// Create a pipe to send data
	HANDLE pipe = CreateNamedPipe(
		L"\\\\.\\pipe\\my_pipe", // name of the pipe
		PIPE_ACCESS_DUPLEX, // 1-way pipe -- send only
		PIPE_TYPE_BYTE, // send data as a byte stream
		1, // only allow 1 instance of this pipe
		0, // no outbound buffer
		0, // no inbound buffer
		0, // use default wait time
		NULL // use default security attributes
	);

	if (pipe == NULL || pipe == INVALID_HANDLE_VALUE) {
		wcout << "Failed to create outbound pipe instance.";
		// look up error code here using GetLastError()
		system("pause");
		return ;
	}
	
	wcout << "Waiting for a client to connect to the pipe..." << endl << endl;

	// This call blocks until a client process connects to the pipe
	BOOL result = ConnectNamedPipe(pipe, NULL);
	if (!result) {
		wcout << "Failed to make connection on named pipe." << endl;
		// look up error code here using GetLastError()
		CloseHandle(pipe); // close the pipe
		system("pause");
		return ;
	}

	std::this_thread::sleep_for(std::chrono::seconds(2));
	wcout << "Sending data to pipe..." << endl;	

	// This call blocks until a client process reads all the data
	const wchar_t *data = L"*** Hello Pipe World ***";
	DWORD numBytesWritten = 0;
	result = WriteFile(
		pipe, // handle to our outbound pipe
		data, // data to send
		wcslen(data) * sizeof(wchar_t), // length of data to send (bytes)
		&numBytesWritten, // will store actual amount of data sent
		NULL // not using overlapped IO
	);

	if (result) {
		wcout << "Number of bytes sent: " << numBytesWritten << endl;
	}
	else {
		wcout << "Failed to send data." << endl;
		// look up error code here using GetLastError()
	}

	DisconnectNamedPipe(pipe);
	// Close the pipe (automatically disconnects client too)
	CloseHandle(pipe);

	wcout << "Done." << endl;
	std::cout << "out NamePipeWrite.\n";
}

void NamePipeRead()
{
	std::cout << "in NamePipeRead.\n";
	wcout << "Connecting to pipe..." << endl;
	std::this_thread::sleep_for(std::chrono::seconds(1));

#if 0
	if (!WaitNamedPipe(L"\\\\127.0.0.1\\pipe\\my_pipe", NMPWAIT_WAIT_FOREVER))
	{
		/*
		WaitNamedPipe, ֻ�йܵ����ڲ��Ҳ������ӵ�����²Ż�ȴ����������̷��ء���ʵûɶ�á�
		It does not wait for a pipe to come into existence, 
		it waits for an existing pipe to become available (i.e. an outstanding ConnectNamedPipe):
		��ʱʱ�����ã�
		nTimeOut �ȴ������ܵ���һ��ʵ����Ч�ĳ�ʱʱ�䣬��λ���룬Ҳ����ʹ����������ֵ�е�һ����
			NMPWAIT_USE_DEFAULT_WAIT 0x00000000,ʹ�÷����CreateNamedPipe �����ܵ�ʱ���õĳ�ʱʱ�䡣
			NMPWAIT_WAIT_FOREVER 0xffffffff��һֱ�ȵ�һ�������ܵ���ʵ����Ч�ŷ��ء�
		*/
		std::cout << "��ǰû�п��õ������ܵ�\n";
		return;
	}
#endif

	// Open the named pipe
	// Most of these parameters aren't very relevant for pipes.
	HANDLE pipe = CreateFile(
		L"\\\\.\\pipe\\my_pipe",
		GENERIC_READ, // only need read access
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);

	if (pipe == INVALID_HANDLE_VALUE) {
		wcout << "Failed to connect to pipe." << endl;
		// look up error code here using GetLastError()
		system("pause");
		return ;
	}

	wcout << "Reading data from pipe..." << endl << endl;

	// The read operation will block until there is data to read
	wchar_t buffer[128];
	DWORD numBytesRead = 0;
	BOOL result = ReadFile(
		pipe,
		buffer, // the data from the pipe will be put here
		127 * sizeof(wchar_t), // number of bytes allocated
		&numBytesRead, // this will store number of bytes actually read
		NULL // not using overlapped IO
	);

	if (result) {
		buffer[numBytesRead / sizeof(wchar_t)] = '\0'; // null terminate the string
		wcout << "Number of bytes read: " << numBytesRead << endl;
		wcout << "Message: " << buffer << endl;
	}
	else {
		wcout << "Failed to read data from the pipe." << endl;
	}

	// Close our pipe handle
	CloseHandle(pipe);

	wcout << "Done." << endl;
	std::cout << "out NamePipeRead.\n";
}

void NamePipeTest()
{
	std::thread w(NamePipeWrite); // server
	std::thread r(NamePipeRead);  // client

	w.join();
	r.join();
}
