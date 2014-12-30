// Class1.cpp
#include "pch.h"
#include "Class1.h"

#include "Threading\ServiceDataPolicy.h"
#include "Threading\ServiceOperation.h"
#include "Threading\ServiceOperationRBase.h"
#include "Threading\ServiceOperationRBaseGeneric.h"
#include "Threading\Service.h"
#include "Threading2\Service.h"

#include <memory>
#include <vector>
#include <functional>
#include <type_traits>

typedef OperationDataPolicyWindows<int> IntDataPolicy;
typedef ServiceOperation<IntDataPolicy> IntOperation;

template<class Derived, class R>
struct IntRServiceOperation
{
	typedef ServiceOperationRBaseRunWrap<IntOperation, Derived, R> Type;
};

typedef ServiceStdThreadPolicy <IntDataPolicy, std::unique_ptr<IntOperation>> IntService;
//typedef ServiceSimplePolicy <IntDataPolicy, std::unique_ptr<IntServiceOperation>> IntService;
//typedef ServiceSimplePolicy <IntDataPolicy, std::shared_ptr<IntServiceOperation>> IntService;
//typedef ServiceSimplePolicy <IntDataPolicy, IntServiceOperation> IntService;
//typedef ServiceSimplePolicy <IntDataPolicy, IntServiceOperation *> IntService;

// data -> operation(data) -> service

class SimpleRCOperation : public IntRServiceOperation<SimpleRCOperation, float>::Type
{
public:

	SimpleRCOperation(){}
	virtual ~SimpleRCOperation(){}

	//RUN_IMPL_METHOD_PROTO;
	RUN_IMPL_METHOD_DEF(SimpleRCOperation)
	{
		//throw std::exception("SimpleRCOperation Hello");
		throw ref new Platform::Exception(-1, "SimpleRCOperation Hello2");

		return 3.3f;
	}
};

int OperationFn(IntDataPolicy &serviceState)
{
	return 123;
}

struct OperationCallableObj
{
	short operator()(IntDataPolicy &serviceState)
	{
		return 12345;
	}
};

auto operationLambda =
[&](IntDataPolicy &serviceState)
{
	return 6.28;
};

std::function<void(IntDataPolicy &serviceState)> operationFunctionObj =
[&](IntDataPolicy &serviceState)
{
	float res = static_cast<float>(operationLambda(serviceState) * 2.0);

	//throw std::exception("Hello");
	throw ref new Platform::Exception(-1, "Hello2");

	//return res;
};

using namespace StopNative;
using namespace Platform;

template<typename T>
using unique_ptr_default = std::unique_ptr<T>;

typedef Threading::ServiceStdThreadPolicy<int, Threading::OperationDataPolicyStd, unique_ptr_default> SP2;

class SimpleOpImpl : public SP2::OperationBaseStd<SimpleOpImpl, float>::Type{
public:

	SimpleOpImpl(){
		//this->SetRethrowException(false);
	}
	virtual ~SimpleOpImpl(){
	}

	float Run(SimpleOpImpl::ServiceType &service){
		auto &data = service.AcquireTaskData();
		service.ReturnTaskData();

		throw std::exception("234");
		//return 2;
		return 1.0f;
	}
};

Class1::Class1()
{
	SP2 sp;

	auto spOp = new SimpleOpImpl;
	auto result = spOp->GetResult();

	std::shared_ptr<Threading::ServiceUnhandledExceptionHandler<Threading::EmptyServiceUnhandledExceptionHandler>> exceptionHandler = std::make_shared<Threading::ServiceUnhandledExceptionHandler<Threading::EmptyServiceUnhandledExceptionHandler>>();

	sp.SetExceptionHandler(exceptionHandler);

	sp.AddOperation(SP2::OperationType(spOp));

	result.GetValue();

	IntService intServ;

	auto opTmp = new SimpleRCOperation;
	auto opResult = opTmp->GetResult();
	std::unique_ptr<IntOperation> simpleIntOp = std::unique_ptr<IntOperation>(opTmp);
	//std::shared_ptr<IntServiceOperation> dd = std::make_shared<IntServiceOperation>();
	/*IntServiceOperation dd;
	IntServiceOperation *p = &dd;*/
	//IntServiceOperation *dd = new IntServiceOperation;

	/*std::vector<std::unique_ptr<IntServiceOperation>> ops;

	ops.push_back(StdCopyPolicy::Copy(dd));*/

	auto tmpRef = StdDerefPolicy::Deref(simpleIntOp);
	intServ.AddOperation(simpleIntOp);

	try
	{
		auto res = opResult.GetValue();
	}
	catch (Platform::Exception ^e)
	{
		int stop = 324;
	}
	catch (const std::exception& e)
	{
		int stop = 324;
	}

	auto fnBasedOp = ServiceOperationRBaseGenericFactory<IntOperation>::Create(OperationFn);
	auto objBasedOp = ServiceOperationRBaseGenericFactory<IntOperation>::Create(OperationCallableObj());
	auto lambdaBasedOp = ServiceOperationRBaseGenericFactory<IntOperation>::Create(operationLambda);
	auto funcObjBasedOp = ServiceOperationRBaseGenericFactory<IntOperation>::Create(operationFunctionObj);

	auto fnBasedOpRes = fnBasedOp->GetResult();
	auto objBasedOpRes = objBasedOp->GetResult();
	auto lambdaBasedOpRes = lambdaBasedOp->GetResult();
	auto funcObjBasedOpRes = funcObjBasedOp->GetResult();

	intServ.AddOperation(std::unique_ptr<IntOperation>(fnBasedOp));
	intServ.AddOperation(std::unique_ptr<IntOperation>(objBasedOp));
	intServ.AddOperation(std::unique_ptr<IntOperation>(lambdaBasedOp));
	intServ.AddOperation(std::unique_ptr<IntOperation>(funcObjBasedOp));

	auto fnRes = fnBasedOpRes.GetValue();
	auto objRes = objBasedOpRes.GetValue();
	auto lambdaRes = lambdaBasedOpRes.GetValue();

	try
	{
		/*auto fnObjRes = */funcObjBasedOpRes.GetValue();
	}
	catch (Platform::Exception ^e)
	{
		int stop = 324;
	}
	catch (const std::exception& e)
	{
		int stop = 324;
	}


	int stop = 324;

}
