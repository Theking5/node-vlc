#define NAPI_CPP_EXCEPTIONS

#include "Player.h"

Napi::Object Player::Init(Napi::Env env, Napi::Object exports) {
	Napi::Function func = DefineClass(env, "Player", {
		InstanceMethod("setVideo", &Player::SetVideo),
		InstanceMethod("play", &Player::Play),
		InstanceMethod("setDimensions", &Player::SetDimensions),
		InstanceMethod("setRenderCallback", &Player::SetRenderCallback)
		});

	Napi::FunctionReference* constructor = new Napi::FunctionReference();
	*constructor = Napi::Persistent(func);

	env.SetInstanceData(constructor);

	exports.Set("Player", func);

	return exports;
}

Player::Player(const Napi::CallbackInfo& info) : Napi::ObjectWrap<Player>(info) {
	this->_libvlc = libvlc_new(0, NULL);
	
	this->_frame = NULL;
	this->_player = NULL;
	this->_videoMem = NULL;
	this->_playing = false;
}

Napi::Value Player::SetVideo(const Napi::CallbackInfo& info) {
	this->_filePath = info[0].As<Napi::String>().Utf8Value();

	if (this->_player != NULL) {
		libvlc_media_player_release(this->_player);
	}

	this->_media = libvlc_media_new_path(this->_libvlc, this->_filePath.c_str());
	this->_player = libvlc_media_player_new_from_media(this->_media);

	libvlc_media_release(this->_media);
	libvlc_video_set_callbacks(this->_player, &Lock, NULL , &Display, this);
	
	return Napi::Value::Value();
}

Napi::Value Player::SetDimensions(const Napi::CallbackInfo& info) {
	float pos;

	this->_width = info[0].As<Napi::Number>().Int32Value();
	this->_height = info[1].As<Napi::Number>().Int32Value();

	if (this->_playing) {
		pos = libvlc_media_player_get_position(this->_player);

		libvlc_media_player_stop(this->_player);
	}

	if (this->_frame != NULL)
		delete this->_frame;

	if (this->_videoMem != NULL)
		delete this->_videoMem;

	int size = this->_width * this->_height * 4;

	this->_frame = new char[size];
	this->_videoMem = new char[size];

	libvlc_video_set_format(this->_player, "RGBA", this->_width, this->_height, this->_width * 4);

	if (this->_playing) {
		libvlc_media_player_play(this->_player);

		libvlc_media_player_set_position(this->_player, pos);
	}

	return Napi::Number::From(info.Env(), this->_height);
}

Napi::Value Player::SetRenderCallback(const Napi::CallbackInfo& info) {
	Napi::Env env = info.Env();

	this->_renderCB = Napi::ThreadSafeFunction::New(env, info[0].As<Napi::Function>(), "Resource Name", 0, 1);

	return Napi::Value::Value();
}

Napi::Value Player::Play(const Napi::CallbackInfo& info) {
	Napi::Env env = info.Env();

	this->_playing = true;

	libvlc_media_player_play(this->_player);

	return Napi::Value::Value();
}

void* Player::Lock(void* opaque, void** planes) {
	Player* player = (Player*)opaque;

	*planes = player->_videoMem;

	return player->_videoMem;
}

void Player::Display(void* opaque, void* picture) {
	Player* player = (Player*)opaque;

	auto callback = [](Napi::Env env, Napi::Function jsCallback, void* opaque) {
		Player* player = (Player*)opaque;

		Napi::ArrayBuffer arr = Napi::ArrayBuffer::New(env, player->_frame, player->_width * player->_height * 4);

		jsCallback.Call({ Napi::Uint8Array::New(env, player->_width * player->_height * 4, arr, 0, napi_typedarray_type::napi_uint8_clamped_array) });
	};

	memcpy(player->_frame, picture, player->_width * player->_height * 4);

	player->_renderCB.BlockingCall(player, callback);
}