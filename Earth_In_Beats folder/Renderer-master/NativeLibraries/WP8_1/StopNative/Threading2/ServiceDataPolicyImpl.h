#pragma once
#include "ServiceDataPolicyBase.h"

#include <mutex>

#ifdef _MSC_VER
#include <Windows.h>

namespace Threading{
	template<class Data>
	class OperationDataPolicyWindows : public ServiceDataPolicyBase < OperationDataPolicyWindows<Data>, Data > {
	public:
		OperationDataPolicyWindows(){
			InitializeCriticalSectionEx(&this->taskDataCS, 3000, 0);
		}

		~OperationDataPolicyWindows(){
			DeleteCriticalSection(&this->taskDataCS);
		}

		Data &AcquireTaskData(){
			EnterCriticalSection(&this->taskDataCS);
			return this->taskData;
		}

		void ReturnTaskData(){
			LeaveCriticalSection(&this->taskDataCS);
		}
	private:
		CRITICAL_SECTION taskDataCS;
		Data taskData;
	};
}
#endif



namespace Threading{
	template<class Data>
	class OperationDataPolicyStd : public ServiceDataPolicyBase < OperationDataPolicyStd<Data>, Data > {
	public:
		OperationDataPolicyStd(){}

		~OperationDataPolicyStd(){}

		Data &AcquireTaskData(){
			this->taskDataMtx.lock();
			return this->taskData;
		}

		void ReturnTaskData(){
			this->taskDataMtx.unlock();
		}
	private:
		std::mutex taskDataMtx;
		Data taskData;
	};
}