#pragma once
#define NAPI_CPP_EXCEPTIONS

#include "Player.h";

struct RenderData {
	void* data;
    Player* player;

	static void RenderData::Free(Napi::Env, void* arr, void* data);
};