#pragma once
#include <Windows.h>
#include <mutex>

template<class Data>
class OperationDataPolicyWindows{
public:
	OperationDataPolicyWindows(){
		InitializeCriticalSectionEx(&this->taskDataCS, 3000, 0);
	}

	Data &AcquireTaskData(){
		EnterCriticalSection(&this->taskDataCS);
		return this->taskData;
	}

	void ReturnTaskData(){
		LeaveCriticalSection(&this->taskDataCS);
	}
//protected:
	// protected because don't need vtable overhead or possible memory leaks
	~OperationDataPolicyWindows(){
		DeleteCriticalSection(&this->taskDataCS);
	}
private:
	CRITICAL_SECTION taskDataCS;
	Data taskData;
};




template<class Data>
class OperationDataPolicyStd{
public:
	OperationDataPolicyStd(){}

	Data &AcquireTaskData(){
		this->taskDataMtx.lock();
		return this->taskData;
	}

	void ReturnTaskData(){
		this->taskDataMtx.unlock();
	}
//protected:
	// protected because don't need vtable overhead or possible memory leaks
	~OperationDataPolicyStd(){}
private:
	std::mutex taskDataMtx;
	Data taskData;
};