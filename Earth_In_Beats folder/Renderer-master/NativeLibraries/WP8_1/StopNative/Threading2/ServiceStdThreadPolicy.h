#pragma once
#include "ServicePolicyBase.h"

#include <list>
#include <queue>
#include <thread>
#include <future>
#include <mutex>
#include <condition_variable>
#include <string>

namespace Threading{
	template<class Data, template<class D> class DataPolicyImpl, template<class T> class OperationWrapper, class ExceptionHandlerImpl = EmptyServiceUnhandledExceptionHandler, class DerefPolicy = StdDerefPolicy, class OpCopyPolicy = StdCopyPolicy>
	class ServiceStdThreadPolicy : public ServicePolicyBase < ServiceStdThreadPolicy<Data, DataPolicyImpl, OperationWrapper, ExceptionHandlerImpl, DerefPolicy>, Data, DataPolicyImpl, OperationWrapper, ExceptionHandlerImpl, DerefPolicy > {
	public:
		ServiceStdThreadPolicy()
			: end(false), completeAllOperationsBeforeDestroy(true){
			this->workThread = std::thread(
				[=](){
				this->Work();
			});
		}

		~ServiceStdThreadPolicy(){
				{
					std::unique_lock<std::mutex> lk(this->operationsMtx);
					this->end = true;
					this->operationsCv.notify_one();
				}
			this->workThread.join();
		}

		void AddOperation(typename ServicePolicyBase::OperationType &op){
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
	private:
		std::thread workThread;
		std::condition_variable operationsCv;
		std::mutex operationsMtx;
		std::queue<typename ServicePolicyBase::OperationType> operations;
		std::mutex currentOpMtx;
		typename ServicePolicyBase::OperationType currentOp;

		bool end;
		bool completeAllOperationsBeforeDestroy;

		void Work(){
			while (this->TryGetNextOperation()){
				if (this->currentOp != nullptr){
					this->RunTask(this->currentOp);
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

		void SetCurOp(typename ServicePolicyBase::OperationType &op){
			std::unique_lock<std::mutex> lk(this->currentOpMtx);
			this->currentOp = OpCopyPolicy::Copy(op);
		}
	};

	// helper class for using class OpCopyPolicy = StdCopyPolicy
	// used as ServiceStdThreadPolicyDef::ServicePolicy
	// if you need to configure OpCopyPolicy you can use ServiceStdThreadPolicyCustom class
	struct ServiceStdThreadPolicyDef{
		template<class Data, template<class D> class DataPolicyImpl, template <class T> class OperationWrapper, class ExceptionHandlerImpl = EmptyServiceUnhandledExceptionHandler, class DerefPolicy = StdDerefPolicy>
		struct ServicePolicy : public ServiceStdThreadPolicy<Data, DataPolicyImpl, OperationWrapper, ExceptionHandlerImpl, DerefPolicy>{};
	};
	// helper class for costumizing only OpCopyPolicy
	// used as ServiceSimplePolicyCustom<MyCopyPolicy>::ServiceDataPolicy
	template<class OpCopyPolicy = StdCopyPolicy>
	struct ServiceStdThreadPolicyCustom{
		template<class Data, template<class D> class DataPolicyImpl, template <class T> class OperationWrapper, class ExceptionHandlerImpl = EmptyServiceUnhandledExceptionHandler, class DerefPolicy = StdDerefPolicy>
		struct ServicePolicy : public ServiceStdThreadPolicy<Data, DataPolicyImpl, OperationWrapper, ExceptionHandlerImpl, DerefPolicy, OpCopyPolicy>{};
	};
}