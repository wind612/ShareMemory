// 共享内存通讯.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include "IPC/ShareMemory.h"
#include <iostream>
#include <nlohmann/json.hpp>

using nlohmann::json;

#ifndef Int32
typedef UINT32 Int32;
#endif // !Int32

struct smm_header
{
	Int32 length;
	Int32 command;
};

struct Person_2
{
	Int32 age;
	Int32 sex;
};

namespace ns {
	// a simple struct to model a person
	struct person {
		std::string name;
		std::string address;
		int age;
		std::vector<int> tmp;
	};

	void to_json(json& j, const person& p) {
		j = json{
			{"name", p.name}, 
			{"address", p.address}, 
			{"age", p.age}, 
			{"tmp", p.tmp} 
		};
	}

	void from_json(const json& j, person& p) {
		j.at("name").get_to(p.name);
		j.at("address").get_to(p.address);
		j.at("age").get_to(p.age);
		j.at("tmp").get_to(p.tmp);
	}

}

CShareMemory g_smm;//如果是局部变量，对象析构之后共享内存里面的东西也会清空。

void test2()
{
	// create a person
	ns::person p{ "Ned Flanders", "744 Evergreen Terrace", 60 , {1,2,3} };

	// conversion: person -> json
	json j = p;
	std::string tmpstr = j.dump();
	const char* pdata = tmpstr.c_str();

	smm_header head;
	head.command = 3;
	head.length = tmpstr.length();
	g_smm.write((BYTE*)&head, 0, sizeof(head));

	g_smm.write((BYTE*)pdata, sizeof(head), tmpstr.length());
	
	printf("write command OK.\r\n");
}

void test1()
{
	smm_header head;	
	g_smm.read((BYTE*)&head, 0, sizeof(head));
	printf("head.length=%d, head.command=%d\r\n", head.length, head.command);

	Person_2 pp;
	if (head.command == 1 && head.length == sizeof(Person_2))
	{
		g_smm.read((BYTE*)&pp, sizeof(smm_header), sizeof(Person_2));
		printf("pp.age=%d, pp.sex=%d\r\n", pp.age, pp.sex);
	}
	

	head.command = 2;
	head.length = 20;
	g_smm.write((BYTE*)&head, 0, sizeof(head));
}

int main()
{
	test2();

	system("pause");
	std::cout << "Hello World!\n";
}


