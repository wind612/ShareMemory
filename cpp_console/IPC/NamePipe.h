#pragma once

#include <windows.h>

class CNamePipe
{
public:
	CNamePipe();
	~CNamePipe();

	void write(BYTE* data, UINT32 data_len);
	void read(BYTE* data, UINT32 data_len);

private:
	HANDLE m_pipe;
};

