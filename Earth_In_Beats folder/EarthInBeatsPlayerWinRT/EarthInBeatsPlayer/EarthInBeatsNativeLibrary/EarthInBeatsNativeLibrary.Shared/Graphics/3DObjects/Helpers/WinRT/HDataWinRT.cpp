#include "HDataWinRT.h"

using namespace Windows::Data::Json;

JsonArray ^HDataWinRT::SavePoint(float x, float y){
	JsonArray ^jsonPoint = ref new JsonArray;

	jsonPoint->Append(JsonValue::CreateNumberValue(x));
	jsonPoint->Append(JsonValue::CreateNumberValue(y));

	return jsonPoint;
}

void HDataWinRT::LoadPoint(JsonArray ^json, float *x, float *y){
	if (x){
		*x = json->GetNumberAt(0);
	}

	if (y){
		*y = json->GetNumberAt(1);
	}
}