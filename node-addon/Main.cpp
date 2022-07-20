#define NAPI_DISABLE_CPP_EXCEPTIONS

#include <napi.h>
#include <vlc/vlc.h>
#include <Windows.h>

#include "Player.h";

Napi::Object Init(Napi::Env env, Napi::Object exports) {
	return Player::Init(env, exports);
}

NODE_API_MODULE(hello, Init)