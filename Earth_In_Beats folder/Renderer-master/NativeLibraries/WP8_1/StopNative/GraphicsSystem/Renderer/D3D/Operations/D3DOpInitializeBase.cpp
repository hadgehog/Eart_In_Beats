#include "pch.h"
#include "D3DOpInitializeBase.h"

D3DOpInitializeBase::D3DOpInitializeBase(){
}

D3DOpInitializeBase::~D3DOpInitializeBase(){
}

void D3DOpInitializeBase::Run(D3DServiceData &serviceData){

	auto &taskData = serviceData.AcquireTaskData();

	serviceData.ReturnTaskData();

	int stop = 432;
}