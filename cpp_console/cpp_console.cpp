// 共享内存通讯.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include "ShareMemory.h"
#include <iostream>

#ifndef Int32
typedef UINT32 Int32;
#endif // !Int32

struct smm_header
{
	Int32 length;
	Int32 command;
};

struct Person
{
	Int32 age;
	Int32 sex;

	//string secret;

};

CShareMemory g_smm;//如果是局部变量，对象析构之后共享内存里面的东西也会清空。


void test1()
{
	smm_header head;	
	g_smm.read((BYTE*)&head, 0, sizeof(head));
	printf("head.length=%d, head.command=%d\r\n", head.length, head.command);

	Person pp;
	if (head.command == 1 && head.length == sizeof(Person))
	{
		g_smm.read((BYTE*)&pp, sizeof(smm_header), sizeof(Person));
		printf("pp.age=%d, pp.sex=%d\r\n", pp.age, pp.sex);
	}
	

	head.command = 2;
	head.length = 20;
	g_smm.write((BYTE*)&head, 0, sizeof(head));
}

int main()
{
	test1();

	system("pause");
	std::cout << "Hello World!\n";
}


