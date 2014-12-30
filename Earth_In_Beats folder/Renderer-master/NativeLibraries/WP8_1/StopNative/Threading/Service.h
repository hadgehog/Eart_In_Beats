#pragma once
#include "ServiceOperation.h"
#include "ServicePolicyHelpers.h"
#include "ServiceSimplePolicy.h"
#include "ServiceStdThreadPolicy.h"

//template<
//	class DataPolicy,
//	template <class> class OperationType,
//	template <class, class> class ExecutionPolicy, 
//>
//class Service : 
//	public ExecutionPolicy<Service<DataPolicy, OperationType, ExecutionPolicy> *, OperationType<Service<DataPolicy, OperationType, ExecutionPolicy> *>>,
//	public DataPolicy
//{
//public:
//
//	Service()
//		: ExecutionPolicy(this)
//	{
//	}
//
//private:
//
//
//};

//template<class ServiceDataPolicy, template <ServiceDataPolicy> class OperationType, template <ServiceDataPolicy> class ExecutionPolicy>
//class Service : public ServiceDataPolicy, public ExecutionPolicy<ServiceDataPolicy>
//{
//public:
//
//	Service()
//	{}
//
//};