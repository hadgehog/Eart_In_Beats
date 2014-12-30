#pragma once

#include <future>

class HTask
{
public:

	static Windows::Foundation::HResult RunSync(Windows::Foundation::IAsyncAction ^action);

	template<class Callable>
	static Windows::Foundation::HResult RunSync(const Callable &c, Windows::UI::Core::CoreDispatcher ^dispatcher, Windows::UI::Core::CoreDispatcherPriority priority = Windows::UI::Core::CoreDispatcherPriority::Normal)
	{
		Windows::Foundation::HResult result;
		Windows::UI::Core::DispatchedHandler ^handler = ref new Windows::UI::Core::DispatchedHandler(c);

		result = HTask::RunSync(dispatcher->RunAsync(priority, handler));

		return result;
	}

};