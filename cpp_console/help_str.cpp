#include "pch.h"
#include "help_str.h"
#include <windows.h>


//1.  utf8转std::string
//转换过程：先将utf8转双字节Unicode编码，再通过WideCharToMultiByte将宽字符转换为多字节。
//----------------------------------------------------------------------- 
std::string UTF8_To_string(const std::string & str)
{
	int nwLen = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);

	wchar_t * pwBuf = new wchar_t[nwLen + 1];//一定要加1，不然会出现尾巴 
	memset(pwBuf, 0, nwLen * 2 + 2);

	MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.length(), pwBuf, nwLen);

	int nLen = WideCharToMultiByte(CP_ACP, 0, pwBuf, -1, NULL, NULL, NULL, NULL);

	char * pBuf = new char[nLen + 1];
	memset(pBuf, 0, nLen + 1);

	WideCharToMultiByte(CP_ACP, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);

	std::string retStr = pBuf;

	delete[]pBuf;
	delete[]pwBuf;

	pBuf = NULL;
	pwBuf = NULL;

	return retStr;
}
////////////////////////////////////////////////////////////////////////// 


////////////////////////////////////////////////////////////////////////// 
//  [8/19/2009 Leezhm] 
//  translate ascii characters to utf-8 characters

//2.std::string转utf8字符串
//转换过程：与1过程相反 
//------------------------------------------------------------------------ 
std::string string_To_UTF8(const std::string & str)
{
	int nwLen = ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);

	wchar_t * pwBuf = new wchar_t[nwLen + 1];//一定要加1，不然会出现尾巴 
	ZeroMemory(pwBuf, nwLen * 2 + 2);

	::MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length(), pwBuf, nwLen);

	int nLen = ::WideCharToMultiByte(CP_UTF8, 0, pwBuf, -1, NULL, NULL, NULL, NULL);

	char * pBuf = new char[nLen + 1];
	ZeroMemory(pBuf, nLen + 1);

	::WideCharToMultiByte(CP_UTF8, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);

	std::string retStr(pBuf);

	delete[]pwBuf;
	delete[]pBuf;

	pwBuf = NULL;
	pBuf = NULL;

	return retStr;
}
//////////////////////////////////////////////////////////////////////////

#include <string>
#include <codecvt>
#include <locale>
#include <iostream>

std::string utf16_to_utf8_std(std::u16string utf16_string) // OK
{
	std::wstring_convert<std::codecvt_utf8_utf16<int16_t>, int16_t> convert;
	auto p = reinterpret_cast<const int16_t *>(utf16_string.data());
	return convert.to_bytes(p, p + utf16_string.size());
}
std::u16string utf8_to_utf16_std(std::string utf8_string) // OK
{
	std::wstring_convert<std::codecvt_utf8_utf16<int16_t>, int16_t> convert;
	auto p = reinterpret_cast<const char *>(utf8_string.data());
	auto str = convert.from_bytes(p, p + utf8_string.size());
	std::u16string u16_str(str.begin(), str.end());
	return u16_str;
}

//////////////////////////////////////////////
// 未验证
typedef std::string u8string;

u8string utf16_to_utf8(const std::u16string &s)
{
	std::wstring_convert<std::codecvt_utf8_utf16<int16_t>, int16_t> convert;
	auto p = reinterpret_cast<const int16_t *>(s.data());
	return convert.to_bytes(p, p + s.size());
}

u8string utf32_to_utf8(const std::u32string &s)
{
	std::wstring_convert<std::codecvt_utf8<int32_t>, int32_t> convert;
	auto p = reinterpret_cast<const int32_t *>(s.data());
	return convert.to_bytes(p, p + s.size());
}

std::u16string utf8_to_utf16(std::string &s)
{
	std::wstring_convert<std::codecvt_utf8_utf16<int16_t>, int16_t> convert;
	auto p = reinterpret_cast<const char *>(s.data());
	auto str = convert.from_bytes(p, p + s.size());
	std::u16string u16_str(str.begin(), str.end());
	return u16_str;
}

std::u16string utf32_to_utf16(std::u32string &s)
{
	std::wstring_convert<std::codecvt_utf8<int32_t>, int32_t> convert;
	auto p = reinterpret_cast<const char *>(s.data());
	auto str = convert.from_bytes(p, p + s.size());
	std::u16string u16_str(str.begin(), str.end());
	return u16_str;
}

//////////
/*
std::u16string To_UTF16(const std::u32string &s)
{
	std::wstring_convert<std::codecvt_utf16<char32_t>, char32_t> conv;
	std::string bytes = conv.to_bytes(s);
	return std::u16string(reinterpret_cast<const char16_t*>(bytes.c_str()), bytes.length() / sizeof(char16_t));
}

std::u32string To_UTF32(const u8string &s)
{
	std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> conv;
	return conv.from_bytes(s);
}

std::u32string To_UTF32(const std::u16string &s)
{
	const char16_t *pData = s.c_str();
	std::wstring_convert<std::codecvt_utf16<char32_t>, char32_t> conv;
	return conv.from_bytes(reinterpret_cast<const char*>(pData), reinterpret_cast<const char*>(pData + s.length()));
}
*/
