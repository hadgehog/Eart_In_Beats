#include "HText.h"
#include "Text\UriCodec.h"
#include "..\config.h"

#include <vector>

#ifdef HAVE_VISUAL_STUDIO
#include <Windows.h>
#endif

std::string HText::UriDecode(const void *src, size_t length){
	return UriCodec::Decode(static_cast<const char *>(src), length);
}

std::string HText::UriEncode(const void *src, size_t length){
	return UriCodec::Encode(static_cast<const char *>(src), length);
}

std::wstring HText::ConvertUTF8ToWString(const std::string& utf8){
#ifdef HAVE_VISUAL_STUDIO
	std::wstring wpath(utf8.length() + 1, '\0');

	// Runtime is modern, narrow calls are widened inside CRT using CP_ACP codepage.
	UINT codepage = CP_UTF8;

	(void) MultiByteToWideChar(codepage, 0, utf8.data(), utf8.length(), &wpath[0], wpath.size());

	auto zeroEnd = wpath.find(L'\0');

	if (zeroEnd != std::wstring::npos){
		wpath.erase(wpath.begin() + zeroEnd, wpath.end());
	}
#elif
#error Add your implementation
#endif

	return wpath;
}

std::string HText::ConvertWStringToUTF8(const std::wstring &s)
{
	std::string result;
#ifdef HAVE_VISUAL_STUDIO
	UINT codepage = CP_UTF8;

	int requiredLength = WideCharToMultiByte(codepage, 0, s.data(), s.size(), nullptr, 0, nullptr, nullptr);

	result.resize(requiredLength);

	if (WideCharToMultiByte(codepage, 0, s.data(), s.size(), const_cast<char *>(result.data()), result.size(), nullptr, nullptr) == 0){
		throw std::exception("Unable to convert Platform::String to std::string!");
	}
#elif
#error Add your implementation
#endif

	// success
	return result;
}

std::string HText::ConvertToUTF8(Platform::String ^v){
	std::string result;
	UINT codepage = CP_UTF8;

	int requiredLength = WideCharToMultiByte(codepage, 0, v->Data(), v->Length(), nullptr, 0, nullptr, nullptr);

	result.resize(requiredLength);

	if (WideCharToMultiByte(codepage, 0, v->Data(), v->Length(), const_cast<char *>(result.data()), result.size(), nullptr, nullptr) == 0){
		throw std::exception("Unable to convert Platform::String to std::string!");
	}

	return result;
}

void HText::BreakNetUrl(const std::wstring &url, std::wstring *protocol, std::wstring *name, std::wstring *port, std::wstring *path){
	if (protocol || name || port || path){
		std::wstring buffer;
		size_t start = 0, i = 0;
		bool havePort = false;

		while (url[i] == ':' || url[i] == '/'){
			i++;
		}
		start = i;

		while (i < url.size()){
			if (url[i] == ':' || url[i] == '/'){
				buffer.push_back(url[i]);
			}
			else{
				if (!buffer.empty()){
					if (buffer.find(L":/") == 0){
						// have protocol
						if (protocol){
							*protocol = std::wstring(url.begin() + start, url.begin() + (i - buffer.size()));
						}
						if (!(name || port || path)){
							break;
						}
					}
					else if (buffer[0] == ':'){
						// port start
						if (name){
							*name = std::wstring(url.begin() + start, url.begin() + (i - buffer.size()));
						}
						havePort = true;
						if (!(port || path)){
							break;
						}
					}
					else if (buffer[0] == '/'){
						// name+path or port+path
						if (havePort){
							if (port){
								*port = std::wstring(url.begin() + start, url.begin() + (i - buffer.size()));
							}
						}
						else{
							if (name){
								*name = std::wstring(url.begin() + start, url.begin() + (i - buffer.size()));
							}
						}
						start = i;
						if (path){
							*path = std::wstring(url.begin() + start, url.end());
						}
						break;
					}

					buffer.clear();
					start = i;
				}
			}
			i++;
		}
	}
}