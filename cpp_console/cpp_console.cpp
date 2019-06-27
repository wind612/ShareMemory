// 共享内存通讯.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include "IPC/ShareMemory.h"
#include "help_str.h"
#include "NamePipeTest.h"
#include "ThreadTest.h"
#include "IPC/NamePipe.h"

#include <iostream>
#include <iomanip>
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
	struct mypoint {
		int x;
		int y;
	};

	struct person {
		std::string name;
		std::string address;
		int age;
		std::vector<int> Tmplist1;
		std::vector<mypoint> Tmplist2;
	};

	void to_json(json& j, const mypoint& p) {
		j = json{
			{"x", p.x},
			{"y", p.y},
		};
	}
	void from_json(const json& j, mypoint& p) {
		j.at("x").get_to(p.x);
		j.at("y").get_to(p.y);
	}

	void to_json(json& j, const person& p) {
		j = json{
			{"name", p.name}, 
			{"address", p.address}, 
			{"age", p.age}, 
			{"Tmplist1", p.Tmplist1},
			{"Tmplist2", p.Tmplist2},
		};
	}

	void from_json(const json& j, person& p) {
		j.at("name").get_to(p.name);
		j.at("address").get_to(p.address);
		j.at("age").get_to(p.age);
		j.at("Tmplist1").get_to(p.Tmplist1);
		j.at("Tmplist2").get_to(p.Tmplist2);
	}

}

CShareMemory g_smm;//如果是局部变量，对象析构之后共享内存里面的东西也会清空。
CNamePipe g_pipe;

void test_read()
{
	// read head
	smm_header head;
	g_smm.read((BYTE*)& head, 0, sizeof(head));
	printf("head.length=%d, head.command=%d\r\n", head.length, head.command);

	// read data.
	char pBuff[1024] = {0};
	g_smm.read((BYTE*)& pBuff, sizeof(smm_header), head.length);
	std::string data_str = pBuff;
	printf("data_str = %s\r\n", data_str.c_str());

	// conversion: json -> person	
	json j = json::parse(data_str);
	auto pp = j.get<ns::person>();	

	// print
	std::string name = UTF8_To_string(pp.name);
	printf("pp.name=%s, pp.address=%s, pp.age=%d\r\n", name.c_str(), pp.address.c_str(), pp.age);

	printf("pp.Tmplist1 = {");
	size_t len = pp.Tmplist1.size();
	for (size_t i = 0; i < len; i++) {
		int d = pp.Tmplist1[i];
		printf("%d, ", d);
	}
	printf("}\r\n");

	printf("pp.Tmplist2 = {");
	len = pp.Tmplist2.size();
	for (size_t i = 0; i < len; i++) {
		auto t2 = pp.Tmplist2[i];
		printf("{x=%d, y=%d}, ", t2.x, t2.y);
	}
	printf("}\r\n");
}

void test_write()
{
	// create a person
	// 对中文的支持
	std::string name = "张三 abc 123";
	std::string name_utf8 = string_To_UTF8(name);
	ns::person p{ name_utf8, "address 1", 60 , {1,2,3}, { {1,1},{2,2},{3,3}} };

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

void test2()
{
	// test write
	g_pipe;
	smm_header head;
	head.command = 3;
	head.length = 30;
	g_pipe.write((BYTE*)&head, sizeof(head));

	// test read	
	g_pipe.read((BYTE*)& head, sizeof(head));
	printf("head.length=%d, head.command=%d\r\n", head.length, head.command);
}

int main()
{
	// Name pipe test.
	//NamePipeTest();
	test2();
	system("pause");
	return 0;

	//std::wstring w = L"{ \"happy\": \"大家好\", \"pi\": 3.141 }";
	std::u32string w = U"[12.2,\"大家好\"]";
	json j2 = json::parse(w);
	auto tmp = j2.dump();
	std::string str111 = UTF8_To_string(tmp);
	std::cout << str111 << std::endl;

	std::string s = R"({"is_delete":0,"is_file":0,"is_share":0,"lv_creator":"","lv_id":1,"lv_size":0,"lv_time":"","name":"回收站","x_id":6,"x_pid":1})";
	std::string utf8str = string_To_UTF8(s);
	json j = json::parse(utf8str);
	utf8str = j.dump();
	std::string str = UTF8_To_string(utf8str);
	std::cout << std::setw(2) << str << std::endl;


	test_read();
	//test_write();

	system("pause");
	std::cout << "Hello World!\n";
}


