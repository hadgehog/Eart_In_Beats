#pragma once
#include "ServiceOperation.h"

#include <memory>

template<class ServiceDataPolicy, class OperationType, class DerefPolicy = StdDerefPolicy>
class ServiceSimplePolicy
{
public:

	ServiceSimplePolicy()
	{}

	void AddOperation(OperationType &op)
	{
		DerefPolicy::Deref(op)->Run(this->serviceData);
	}

//protected:

	~ServiceSimplePolicy()
	{}

private:

	ServiceDataPolicy serviceData;

};