#pragma once
#include "ServicePolicyBase.h"

namespace Threading{
	template<
		class Impl,
		class Data, template<class D> class DataPolicyImpl,
			template <class T> class OperationWrapper,
		class ExceptionHandlerImpl,
		class DerefPolicy = StdDerefPolicy>
	class _ServiceOperationBase{
	public:
		typedef ServicePolicyBase<Impl, Data, DataPolicyImpl, OperationWrapper, ExceptionHandlerImpl, DerefPolicy> ServiceType;

		virtual ~_ServiceOperationBase() {}

		bool IsCancelled() const{
			return this->cancelled;
		}

		void Cancel(){
			this->cancelled = true;
		}

		virtual void RunVirtual(ServiceType &service) = 0;
	protected:
		_ServiceOperationBase()
			: cancelled(false){}
	private:
		bool cancelled;
	};




	template<
		class Impl, class ServImpl,
		class Data, template<class D> class DataPolicyImpl,
		class Ret, template <class Ret> class RetPolicyContainerImpl, template <class Ret, class RetPolicyContainerImpl> class RetPolicyImpl,
		class ExceptionHandlerImpl,
			template <class T> class OperationWrapper,
		class DerefPolicy = StdDerefPolicy>
	class ServiceOperationBase : public _ServiceOperationBase < ServImpl, Data, DataPolicyImpl, OperationWrapper, ExceptionHandlerImpl, DerefPolicy > {
	public:
		virtual ~ServiceOperationBase() {}

		ReturnContainerBase<RetPolicyContainerImpl<Ret>, Ret> GetResult(){
			return this->retPolicy.GetResult();
		}

		virtual void RunVirtual(ServiceType &service) override{
			try{
				this->retPolicy.SetResult(this->Run(service));
			}
			catch (...){
				this->retPolicy.SetException(std::current_exception());
				if (this->GetRethrowException()){
					std::rethrow_exception(std::current_exception());
				}
			}
		}

		Ret Run(ServiceType &service){
			return static_cast<Impl*>(this)->Run(service);
		}
	protected:
		ServiceOperationBase()
			: retPolicy(_retPolicy), rethrowException(true){
		}

		bool GetRethrowException() const{
			return this->rethrowException;
		}

		void SetRethrowException(bool v){
			this->rethrowException = v;
		}
	private:
		typedef RetPolicyImpl<Ret, RetPolicyContainerImpl<Ret>> RetPolicyImplType;
		RetPolicyImplType _retPolicy;
		ServiceOperationReturnPolicyBase<RetPolicyImplType, RetPolicyContainerImpl<Ret>, Ret> &retPolicy;

		bool rethrowException;
	};

	// void specialization
	template<
		class Impl, class ServImpl,
		class Data, template<class D> class DataPolicyImpl,
		template <class Ret> class RetPolicyContainerImpl, template <class Ret, class RetPolicyContainerImpl> class RetPolicyImpl,
		class ExceptionHandlerImpl,
			template <class T> class OperationWrapper,
		class DerefPolicy>
	class ServiceOperationBase<
		Impl, ServImpl, 
		Data, DataPolicyImpl, 
		void, RetPolicyContainerImpl, RetPolicyImpl, 
		ExceptionHandlerImpl, 
		OperationWrapper, 
		DerefPolicy> : public _ServiceOperationBase < ServImpl, Data, DataPolicyImpl, OperationWrapper, ExceptionHandlerImpl, DerefPolicy >{
	public:
		virtual ~ServiceOperationBase() {}

		ReturnContainerBase<RetPolicyContainerImpl<void>, void> GetResult(){
			return this->retPolicy.GetResult();
		}

		virtual void RunVirtual(ServiceType &service) override{
			try{
				this->Run(service);
				this->retPolicy.SetResult();
			}
			catch (...){
				this->retPolicy.SetException(std::current_exception());
				if (this->GetRethrowException()){
					std::rethrow_exception(std::current_exception());
				}
			}
		}

		void Run(ServiceType &service){
			static_cast<Impl*>(this)->Run(service);
		}
	protected:
		ServiceOperationBase()
			: retPolicy(_retPolicy), rethrowException(true){
		}

		bool GetRethrowException() const{
			return this->rethrowException;
		}

		void SetRethrowException(bool v){
			this->rethrowException = v;
		}
	private:
		typedef RetPolicyImpl<void, RetPolicyContainerImpl<void>> RetPolicyImplType;
		RetPolicyImplType _retPolicy;
		ServiceOperationReturnPolicyBase<RetPolicyImplType, RetPolicyContainerImpl<void>, void> &retPolicy;

		bool rethrowException;
	};
}