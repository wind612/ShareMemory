// 共享内存通讯.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include "ShareMemory.h"
#include <iostream>


struct Person
{
	UINT32 age;

	UINT32 sex;

	//string secret;

};

CShareMemory g_smm;

void test1()
{
	BYTE data[10] = { 1,2,3,4,5,6,7,8,9,10 };
	BYTE data_2[10];

	//tmp.test();

	g_smm.write(data, 10);

	int readlen = g_smm.read(data_2, 10);
	for (int i = 0; i < readlen; i++)
	{
		std::cout << dec << (int)data_2[i] << ",";
	}
	std::cout << endl;
}

void test2()
{
	Person pp;
	pp.age = 11;
	pp.sex = 1;

	BYTE data[100];
	BYTE data_2[100];

	memcpy(data, (void*)&pp, sizeof(pp));

	g_smm.write(data, sizeof(pp));

	int readlen = g_smm.read(data_2, 10);
	for (int i = 0; i < readlen; i++)
	{
		std::cout << dec << (int)data_2[i] << ",";
	}
	std::cout << endl;
}

int main()
{
	test2();

	system("pause");
	std::cout << "Hello World!\n";
}


