#include "pch.h"
#include "HTask.h"

using namespace Windows::Foundation;

HResult HTask::RunSync(IAsyncAction ^action)
{
	HResult result;
	std::promise<HResult> prom;
	std::future<HResult> fut = prom.get_future();
	
	action->Completed = ref new AsyncActionCompletedHandler(
		[&](IAsyncAction ^action, AsyncStatus asyncStatus)
	{
		prom.set_value(action->ErrorCode);
	});

	result = fut.get();

	return result;
}