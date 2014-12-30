#pragma once
#include "ServiceOperation.h"
#include "ServiceOperationReturnPolicy.h"

template<class ServiceOperationBase,
class R = void, template <class T> class ReturnPolicy = StdReturnPolicy>
class ServiceOperationRBase : 
	public ServiceOperationBase,
	public ReturnPolicy<R>
{
public:

	typedef ServiceOperationBase Base;

	ServiceOperationRBase() {}

	virtual ~ServiceOperationRBase() {}
};

//http://stackoverflow.com/questions/25661817/using-typedefs-of-a-non-template-derived-class-in-the-base-class-when-using-crtp
//template <class ServiceOperationDerived>
//struct ServiceOperationRBaseTraits;

#define RUN_IMPL_METHOD_PROTO RunImplRetType RunImpl(DataPolicy &serviceState)
#define RUN_IMPL_METHOD_DEF(Implementor) Implementor::RunImplRetType Implementor::RunImpl(Implementor::DataPolicy &serviceState)

template<class ServiceOperationBase, class ServiceOperationDerived,
class R = void,
template <class T> class ReturnPolicy = StdReturnPolicy>
class ServiceOperationRBaseRunWrap :
	public ServiceOperationBase,
	public ReturnPolicy<R>
{
public:

	typedef R RunImplRetType;
	typedef ServiceOperationBase Base;

	ServiceOperationRBaseRunWrap() {}

	virtual ~ServiceOperationRBaseRunWrap() {}

	virtual void Run(typename ServiceOperationBase::DataPolicy &serviceState) override
	{
		typedef typename decltype(std::declval<typedef typename ServiceOperationDerived>().RunImpl(std::declval<ServiceOperationBase::DataPolicy &>())) RealRunImplRetType;
		static_assert(std::is_same<RealRunImplRetType, RunImplRetType>::value, "Wrong return type of RunImpl method");

		try
		{
			this->SetResult(static_cast<ServiceOperationDerived*>(this)->RunImpl(serviceState));
		}
		catch (...)
		{
			try
			{
				// set_exception() may throw too
				this->SetException(std::current_exception());
			}
			catch (...)
			{
				int stop = 324;
			}
		}
	}
};


template<class ServiceOperationBase, class ServiceOperationDerived,
	template <class T> class ReturnPolicy>
class ServiceOperationRBaseRunWrap<ServiceOperationBase, ServiceOperationDerived, void, ReturnPolicy> :
	public ServiceOperationBase,
	public ReturnPolicy<void>
{
public:

	typedef void RunImplRetType;
	typedef ServiceOperationBase Base;

	ServiceOperationRBaseRunWrap() {}

	virtual ~ServiceOperationRBaseRunWrap() {}

	virtual void Run(typename ServiceOperationBase::DataPolicy &serviceState) override
	{
		typedef typename decltype(std::declval<typedef typename ServiceOperationDerived>().RunImpl(std::declval<ServiceOperationBase::DataPolicy &>())) RealRunImplRetType;
		static_assert(std::is_same<RealRunImplRetType, RunImplRetType>::value, "Wrong return type of RunImpl method");

		try
		{
			static_cast<ServiceOperationDerived*>(this)->RunImpl(serviceState);
			this->SetResult();
		}
		catch (...)
		{
			try
			{
				// set_exception() may throw too
				this->SetException(std::current_exception());
			}
			catch (...)
			{
				int stop = 324;
			}
		}
	}
};