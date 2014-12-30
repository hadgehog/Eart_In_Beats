#pragma once
#include "ServicePolicyBase.h"

namespace Threading{
	template<class Data, template<class D> class DataPolicyImpl, template<class T> class OperationWrapper, class ExceptionHandlerImpl = EmptyServiceUnhandledExceptionHandler, class DerefPolicy = StdDerefPolicy>
	class ServiceSimplePolicy : public ServicePolicyBase < ServiceSimplePolicy<Data, DataPolicyImpl, OperationWrapper, ExceptionHandlerImpl, DerefPolicy>, Data, DataPolicyImpl, OperationWrapper, ExceptionHandlerImpl, DerefPolicy > {
	public:
		ServiceSimplePolicy(){}

		void AddOperationImpl(typename ServicePolicyBase::OperationType &op){
			this->RunTask(op);
		}

		~ServiceSimplePolicy(){}
	};
}