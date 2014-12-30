#pragma once
#include "ServiceOperationReturnPolicyBase.h"

#include <future>

namespace Threading{
	template<class R>
	class StdReturnContainer{
	public:
		void SetFuture(std::shared_future<R> &fut){
			this->fut = fut;
		}

		R GetValue(){
			return this->fut.get();
		}
	private:
		std::shared_future<R> fut;
	};

	template<>
	class StdReturnContainer < void > {
	public:
		void SetFuture(std::shared_future<void> &fut){
			this->fut = fut;
		}

		void GetValue(){
			this->fut.get();
		}
	private:
		std::shared_future<void> fut;
	};




	template<class R, class ContainerImpl = StdReturnContainer<R>>
	class StdReturnPolicy : public ServiceOperationReturnPolicyBase < StdReturnPolicy<R, ContainerImpl>, ContainerImpl, R > {
	public:
		StdReturnPolicy(){
			this->fut = this->prom.get_future();
		}

		~StdReturnPolicy(){}

		ReturnContainerBase<ContainerImpl, R> GetResult(){
			ReturnContainerBase<ContainerImpl, R> res;
			res.impl.SetFuture(this->fut);
			return res;
		}

		void SetResult(R &&v){
			this->prom.set_value(v);
		}

		void SetException(std::exception_ptr e){
			this->prom.set_exception(e);
		}
	private:
		std::promise<R> prom;
		std::shared_future<R> fut;
	};

	template<class ContainerImpl>
	class StdReturnPolicy<void, ContainerImpl> : public ServiceOperationReturnPolicyBase < StdReturnPolicy<void, ContainerImpl>, ContainerImpl, void > {
	public:
		StdReturnPolicy(){
			this->fut = this->prom.get_future();
		}

		~StdReturnPolicy(){}

		ReturnContainerBase<ContainerImpl, void> GetResult(){
			ReturnContainerBase<ContainerImpl, void> res;
			res.impl.SetFuture(this->fut);
			return res;
		}

		void SetResult(){
			this->prom.set_value();
		}

		void SetException(std::exception_ptr e){
			this->prom.set_exception(e);
		}
	private:
		std::promise<void> prom;
		std::shared_future<void> fut;
	};
}