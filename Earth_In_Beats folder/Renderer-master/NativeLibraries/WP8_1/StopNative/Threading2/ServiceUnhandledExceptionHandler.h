#pragma once
#include <exception>

namespace Threading{
	template<class Impl>
	class ServiceUnhandledExceptionHandler{
	public:
		Impl impl;

		void HandleException(std::exception_ptr exception){
			impl.HandleException(exception);
		}
	};

	class EmptyServiceUnhandledExceptionHandler{
	public:
		void HandleException(std::exception_ptr exception){
			int stop = 243;
		}
	};
}