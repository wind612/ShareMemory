#include "pch.h"
#include "NamePipe.h"
#include <iostream>
#include <thread>
#include <windows.h>

using namespace std;

CNamePipe::CNamePipe()
{
	// Create a pipe to send data
	m_pipe = CreateNamedPipe(
		L"\\\\.\\pipe\\PipeCppCshap", // name of the pipe
		PIPE_ACCESS_DUPLEX, // 1-way pipe -- send and receive
		PIPE_TYPE_BYTE, // send data as a byte stream
		1, // only allow 1 instance of this pipe
		0, // no outbound buffer
		0, // no inbound buffer
		0, // use default wait time
		NULL // use default security attributes
	);

	if (m_pipe == NULL || m_pipe == INVALID_HANDLE_VALUE) {
		wcout << "Failed to create outbound pipe instance.";
		// look up error code here using GetLastError()
		system("pause");
		return;
	}

	wcout << "Waiting for a client to connect to the pipe..." << endl << endl;

	// This call blocks until a client process connects to the pipe
	BOOL result = ConnectNamedPipe(m_pipe, NULL);
	if (!result) {
		wcout << "Failed to make connection on named pipe." << endl;
		// look up error code here using GetLastError()
		CloseHandle(m_pipe); // close the pipe
	}
}

CNamePipe::~CNamePipe()
{
	DisconnectNamedPipe(m_pipe);
	// Close the pipe (automatically disconnects client too)
	CloseHandle(m_pipe);
}

void CNamePipe::write(BYTE* data,  UINT32 data_len)
{
	DWORD numBytesWritten = 0;
	BOOL result = WriteFile(
		m_pipe, // handle to our outbound pipe
		data, // data to send
		data_len, // length of data to send (bytes)
		&numBytesWritten, // will store actual amount of data sent
		NULL // not using overlapped IO
	);
}

void CNamePipe::read(BYTE* data,  UINT32 data_len)
{
	DWORD numBytesRead = 0;
	BOOL result = ReadFile(
		m_pipe,
		data, // the data from the pipe will be put here
		data_len, // number of bytes allocated
		&numBytesRead, // this will store number of bytes actually read
		NULL // not using overlapped IO
	);
}

