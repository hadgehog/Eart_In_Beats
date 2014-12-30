#pragma once

#include <exception>

namespace Threading{
	template<class Impl, class R>
	class ReturnContainerBase{
	public:
		Impl impl;

		R GetValue(){
			return impl.GetValue();
		}
	};

	template<class Impl>
	class ReturnContainerBase < Impl, void > {
	public:
		Impl impl;

		void GetValue(){
			impl.GetValue();
		}
	};




	template<class Impl, class ContainerImpl, class R>
	class ServiceOperationReturnPolicyBase{
	public:
		typedef Impl Impl;
		//typedef typename ReturnContainerBase<ContainerImpl, R> ReturnType;

		ReturnContainerBase<ContainerImpl, R> GetResult(){
			return static_cast<Impl*>(this)->GetResult();
		}

		void SetResult(R &&v){
			static_cast<Impl*>(this)->SetResult(std::move(v));
		}

		void SetException(std::exception_ptr e){
			static_cast<Impl*>(this)->SetException(e);
		}
	protected:
		~ServiceOperationReturnPolicyBase(){}
	};

	template<class Impl, class ContainerImpl>
	class ServiceOperationReturnPolicyBase < Impl, ContainerImpl, void > {
	public:
		typedef Impl Impl;

		ReturnContainerBase<ContainerImpl, void> GetResult(){
			return static_cast<Impl*>(this)->GetResult();
		}

		void SetResult(){
			static_cast<Impl*>(this)->SetResult();
		}

		void SetException(std::exception_ptr e){
			static_cast<Impl*>(this)->SetException(e);
		}
	protected:
		~ServiceOperationReturnPolicyBase(){}
	};
}