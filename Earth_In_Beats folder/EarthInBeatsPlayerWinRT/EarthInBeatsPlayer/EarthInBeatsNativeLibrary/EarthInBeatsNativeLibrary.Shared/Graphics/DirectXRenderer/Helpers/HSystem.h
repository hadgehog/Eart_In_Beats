#pragma once
#include "..\config.h"

#include <cstdint>
#include <string>
#include <future>
#include <memory>
#include <functional>

#ifdef HAVE_VISUAL_STUDIO
#include <intrin.h>
#include <windows.h>
#endif

class HSystem{
public:
	static void DebuggerBreak();
	static void Assert(bool expression);
	static void DebugOutput(const std::string &v);
	static std::vector<uint8_t> LoadPackageFile(const std::wstring &path);

	static std::wstring FixPathWinRT(const std::wstring &v);

	static void ThrowIfFailed(HRESULT hr);

	static HRESULT PerformSync(Windows::Foundation::IAsyncAction ^op);

	template<typename T>
	static HRESULT PerformSync(Windows::Foundation::IAsyncActionWithProgress<T> ^op){
		std::promise<HRESULT> prom;
		auto fut = prom.get_future();

		op->Completed = ref new Windows::Foundation::AsyncActionWithProgressCompletedHandler<T>(
			[&](Windows::Foundation::IAsyncActionWithProgress<T> ^op, Windows::Foundation::AsyncStatus status){
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

	template<typename T>
	static std::pair<HRESULT, T> PerformSync(Windows::Foundation::IAsyncOperation<T> ^op){
		std::promise<std::pair<HRESULT, T>> prom;	//break exception there
		auto fut = prom.get_future();

		op->Completed = ref new Windows::Foundation::AsyncOperationCompletedHandler<T>(
			[&](Windows::Foundation::IAsyncOperation<T> ^op, Windows::Foundation::AsyncStatus status){
			std::pair<HRESULT, T> res;

			if (status == Windows::Foundation::AsyncStatus::Completed){
				res.first = S_OK;
				res.second = op->GetResults();
			}
			else{
				res.first = static_cast<HRESULT>(op->ErrorCode.Value);
				res.second = T();
			}

			prom.set_value(res);
		});

		auto result = fut.get();

		return result;
	}

	template<typename T, typename T2>
	static std::pair<HRESULT, T> PerformSync(Windows::Foundation::IAsyncOperationWithProgress<T, T2> ^op){
		std::promise<std::pair<HRESULT, T>> prom;
		auto fut = prom.get_future();

		op->Completed = ref new Windows::Foundation::AsyncOperationWithProgressCompletedHandler<T, T2>(
			[&](Windows::Foundation::IAsyncOperationWithProgress<T, T2> ^op, Windows::Foundation::AsyncStatus status){
			std::pair<HRESULT, T> res;

			if (status == Windows::Foundation::AsyncStatus::Completed){
				res.first = S_OK;
				res.second = op->GetResults();
			}
			else{
				res.first = static_cast<HRESULT>(op->ErrorCode.Value);
				res.second = T();
			}

			prom.set_value(res);
		});

		auto result = fut.get();

		return result;
	}
};

#ifdef HAVE_VISUAL_STUDIO

struct GUIDHash{
	std::size_t operator()(const GUID &v) const;
};

#endif