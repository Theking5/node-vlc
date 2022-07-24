#include "RenderData.h"
#include <iostream>

void RenderData::Free(Napi::Env, void* arr, void* data) {
	RenderData* renderData = (RenderData*)data;

	//delete renderData->data;
	delete renderData;
}