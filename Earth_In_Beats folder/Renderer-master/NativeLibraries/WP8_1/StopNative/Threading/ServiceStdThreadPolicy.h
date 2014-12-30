#pragma once
#include "ServiceOperation.h"

#include <list>
#include <queue>
#include <thread>
#include <future>
#include <mutex>
#include <condition_variable>
#include <string>

template<class ServiceDataPolicy, class OperationType, class DerefPolicy = StdDerefPolicy, class OpCopyPolicy = StdCopyPolicy>
class ServiceStdThreadPolicy{
public:
	ServiceStdThreadPolicy()
		: end(false), completeAllOperationsBeforeDestroy(true){
		this->workThread = std::thread(
			[=](){
			this->Work();
		});
	}

	void AddOperation(OperationType &op){
		std::unique_lock<std::mutex> lk(this->operationsMtx);
		this->operations.push(OpCopyPolicy::Copy(op));
		this->operationsCv.notify_one();
	}

	uint32_t GetOperationCount(){
		std::unique_lock<std::mutex> lk(this->operationsMtx);
		uint32_t opCount = this->operations.size();
		return opCount;
	}

	bool GetCompleteAllOperationsBeforeDestroy(){
		std::unique_lock<std::mutex> lk(this->operationsMtx);
		return this->completeAllOperationsBeforeDestroy;
	}

	void SetCompleteAllOperationsBeforeDestroy(bool v){
		std::unique_lock<std::mutex> lk(this->operationsMtx);
		this->completeAllOperationsBeforeDestroy = v;
	}

	void ClearOperations(){
		std::unique_lock<std::mutex> lk(this->operationsMtx);
		this->operations = std::queue<OperationType>();
	}

//protected:

	~ServiceStdThreadPolicy(){
		{
			std::unique_lock<std::mutex> lk(this->operationsMtx);
			this->end = true;
			this->operationsCv.notify_one();
		}
		this->workThread.join();
	}

private:

	ServiceDataPolicy serviceData;

	std::thread workThread;
	std::condition_variable operationsCv;
	std::mutex operationsMtx;
	std::queue<OperationType> operations;
	std::mutex currentOpMtx;
	OperationType currentOp;

	bool end;
	bool completeAllOperationsBeforeDestroy;

	void Work(){
		while (this->TryGetNextOperation()){
			if (this->currentOp != nullptr){
				DerefPolicy::Deref(this->currentOp)->Run(this->serviceData);
				this->SetCurOp(OperationType());
			}
		}
	}

	bool TryGetNextOperation(){
		bool needWork = true;
		std::unique_lock<std::mutex> lk(this->operationsMtx);

		while (this->operations.empty() && !this->end){
			this->operationsCv.wait(lk);
		}

		needWork = this->end == false;

		if (!this->operations.empty()){
			this->SetCurOp(this->operations.front());
			this->operations.pop();

			if (!needWork){
				needWork = this->completeAllOperationsBeforeDestroy;
			}
		}

		return needWork;
	}

	void SetCurOp(OperationType &op){
		std::unique_lock<std::mutex> lk(this->currentOpMtx);
		this->currentOp = OpCopyPolicy::Copy(op);
	}
};