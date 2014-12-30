#pragma once
#include "ServiceOperationBase.h"
#include "ServiceDataPolicyBase.h"
#include "ServicePolicyHelpersImpl.h"
#include "ServiceOperationStdReturnPolicy.h"
#include "ServiceUnhandledExceptionHandler.h"

#include <memory>

namespace Threading{
	template<
		class Impl,
		class Data, template<class D> class DataPolicyImpl,
			template <class T> class OperationWrapper,
		class ExceptionHandlerImpl,
		class DerefPolicy>
	class _ServiceOperationBase;

	template<
		class Impl, class ServImpl,
		class Data, template<class D> class DataPolicyImpl,
		class Ret, template <class Ret> class RetPolicyContainerImpl, template <class Ret, class RetPolicyContainerImpl> class RetPolicyImpl,
		class ExceptionHandlerImpl,
			template <class T> class OperationWrapper,
		class DerefPolicy>
	class ServiceOperationBase;

	template<class Impl, class Data, template<class D> class DataPolicyImpl, template <class T> class OperationWrapper, class ExceptionHandlerImpl = EmptyServiceUnhandledExceptionHandler, class DerefPolicy = StdDerefPolicy>
	class ServicePolicyBase{
	public:
		typedef Impl Impl;
		typedef ServiceUnhandledExceptionHandler<ExceptionHandlerImpl> ExceptionHandlerType;
		typedef OperationWrapper<_ServiceOperationBase<Impl, Data, DataPolicyImpl, OperationWrapper, ExceptionHandlerImpl, DerefPolicy>> OperationType;

		template<
			class OpImpl,
			class Ret, template <class Ret> class RetPolicyContainerImpl, template <class Ret, class RetPolicyContainerImpl> class RetPolicyImpl>
		struct OperationBase{
			typedef ServiceOperationBase<
				OpImpl, Impl,
				Data, DataPolicyImpl,
				Ret, RetPolicyContainerImpl, RetPolicyImpl,
				ExceptionHandlerImpl,
				OperationWrapper,
				DerefPolicy> Type;
		};

		template<
			class OpImpl,
			class Ret>
		struct OperationBaseStd{
			typedef ServiceOperationBase<
				OpImpl, Impl,
				Data, DataPolicyImpl,
				Ret, StdReturnContainer, StdReturnPolicy,
				ExceptionHandlerImpl,
				OperationWrapper,
				DerefPolicy> Type;
		};

		ServicePolicyBase()
			: serviceData(_serviceData){}

		void SetExceptionHandler(const std::shared_ptr<ExceptionHandlerType> &handler){
			this->exceptionHandler = handler;
		}

		void AddOperation(OperationType &op){
			static_cast<Impl*>(this)->AddOperationImpl(op);
		}

		Data &AcquireTaskData(){
			return this->serviceData.AcquireTaskData();
		}

		void ReturnTaskData(){
			this->serviceData.ReturnTaskData();
		}

	protected:
		DataPolicyImpl<Data> _serviceData;
		ServiceDataPolicyBase<DataPolicyImpl<Data>, Data> &serviceData;
		std::shared_ptr<ExceptionHandlerType> exceptionHandler;

		~ServicePolicyBase(){}

		void RunTask(OperationType &op){
			try{
				DerefPolicy::Deref(op)->RunVirtual(*this);
			}
			catch (...){
				this->exceptionHandler->HandleException(std::current_exception());
			}
		}
	};
}