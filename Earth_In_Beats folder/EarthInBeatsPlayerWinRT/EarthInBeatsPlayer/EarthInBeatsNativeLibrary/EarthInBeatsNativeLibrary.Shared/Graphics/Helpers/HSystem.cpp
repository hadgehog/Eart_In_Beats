#include "HSystem.h"
#include "HData.h"

#include <assert.h>

void HSystem::DebuggerBreak(){
#if defined(HAVE_VISUAL_STUDIO) && defined(_DEBUG)
	__debugbreak();
#else
	// Just place breakpoint on this line:
	int stop = 234;
#endif
}

void HSystem::Assert(bool expression){
	if (!expression){
		HSystem::DebuggerBreak();
	}
	assert(expression);
}

void HSystem::DebugOutput(const std::string &v){
#ifdef _DEBUG
#ifdef HAVE_VISUAL_STUDIO
	OutputDebugStringA("\n");
	OutputDebugStringA(v.c_str());
#else
	printf("\n%s", v.c_str());
#endif
#endif
}

std::vector<uint8_t> HSystem::LoadPackageFile(const std::wstring &path){
	std::vector<uint8_t> fileData;
	auto tmpPath = Windows::ApplicationModel::Package::Current->InstalledLocation->Path;
	std::wstring installedPath(tmpPath->Data(), tmpPath->Length());
	std::wstring fullPath = installedPath + L"\\" + path;

	auto file = CreateFile2(fullPath.c_str(), GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING, nullptr);

	if (file != INVALID_HANDLE_VALUE){
		DWORD readed;
		LARGE_INTEGER pos, newPos, fileSize;

		pos.QuadPart = 0;
		newPos.QuadPart = 0;

		SetFilePointerEx(file, pos, &newPos, FILE_END);
		fileSize = newPos;
		SetFilePointerEx(file, pos, &newPos, FILE_BEGIN);

		fileData.resize(static_cast<size_t>(fileSize.QuadPart));

		auto res = ReadFile(file, fileData.data(), fileData.size(), &readed, nullptr);

		CloseHandle(file);
	}

	return fileData;
}

std::wstring HSystem::FixPathWinRT(const std::wstring &v){
	std::wstring fixed;

	if (!v.empty()){
		size_t startIdx = 0;
		size_t size = v.size();

		if (v[startIdx] == '\\' || v[startIdx] == '/'){
			startIdx++;
		}

		if (v[size - 1] == '\\' || v[size - 1] == '/'){
			size--;
		}

		fixed.reserve(size);

		for (size_t i = startIdx; i < size; i++){
			if (v[i] != '/'){
				fixed.push_back(v[i]);
			}
			else{
				fixed.push_back('\\');
			}
		}
	}

	return fixed;
}

void HSystem::ThrowIfFailed(HRESULT hr){
	if (FAILED(hr)){
		throw ref new Platform::Exception(hr);
	}
}

HRESULT HSystem::PerformSync(Windows::Foundation::IAsyncAction ^op){
	std::promise<HRESULT> prom;
	auto fut = prom.get_future();

	op->Completed = ref new Windows::Foundation::AsyncActionCompletedHandler(
		[&](Windows::Foundation::IAsyncAction ^op, Windows::Foundation::AsyncStatus status){
		HRESULT res;

		if (status == Windows::Foundation::AsyncStatus::Completed){
			res = S_OK;
		}
		else{
			res = static_cast<HRESULT>(op->ErrorCode.Value);
		}

		prom.set_value(res);
	});

	auto result = fut.get();

	return result;
}




#ifdef HAVE_VISUAL_STUDIO

std::size_t GUIDHash::operator()(const GUID &v) const{
	const uint64_t *tmpData4 = reinterpret_cast<const uint64_t *>(v.Data4);

	std::size_t h1 = std::hash<unsigned long>()(v.Data1);
	std::size_t h2 = std::hash<unsigned short>()(v.Data2);
	std::size_t h3 = std::hash<unsigned short>()(v.Data3);
	std::size_t h4 = std::hash<uint64_t>()(*tmpData4);

	return HData::CombineHash(h1, HData::CombineHash(h2, HData::CombineHash(h3, h4)));
}

#endif