#pragma once
#include "..\..\..\Threading\Service.h"
#include "..\..\..\Threading\ServiceDataPolicy.h"
#include "..\..\..\Threading\ServiceOperation.h"
#include "..\..\..\Threading\ServiceOperationRBase.h"
#include "..\..\..\Threading\ServiceOperationRBaseGeneric.h"
#include "RendererD3DServiceData.h"

#include <memory>

typedef OperationDataPolicyWindows<RendererD3DServiceData> D3DServiceData;
typedef ServiceOperation<D3DServiceData> D3DOperation;

template<class Derived, class R>
struct D3DRServiceOperation{
	typedef ServiceOperationRBaseRunWrap<D3DOperation, Derived, R> Type;
};

typedef ServiceStdThreadPolicy <D3DServiceData, std::unique_ptr<D3DOperation>> D3DService;