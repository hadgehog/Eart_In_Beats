#pragma once

namespace Threading{
	template<class Impl, class Data>
	class ServiceDataPolicyBase{
	public:
		typedef Impl Impl;

		Data &AcquireTaskData(){
			return static_cast<Impl*>(this)->AcquireTaskData();
		}

		void ReturnTaskData(){
			static_cast<Impl*>(this)->ReturnTaskData();
		}
	protected:
		ServiceDataPolicyBase(){}
		// protected because don't need vtable overhead or possible memory leaks
		~ServiceDataPolicyBase(){}
	};
}