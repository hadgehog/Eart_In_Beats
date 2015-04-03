#pragma once

#include <string>

class HText{
public:
	static std::string UriDecode(const void *src, size_t length);
	static std::string UriEncode(const void *src, size_t length);

	static std::wstring ConvertUTF8ToWString(const std::string &utf8);
	static std::string ConvertWStringToUTF8(const std::wstring &s);
	static std::string ConvertToUTF8(Platform::String ^v);

	static void BreakNetUrl(const std::wstring &url, std::wstring *protocol, std::wstring *name, std::wstring *port, std::wstring *path);
};