#define NAPI_CPP_EXCEPTIONS

#include "Player.h"
#include "RenderData.h";

#include <iostream>


Napi::Object Player::Init(Napi::Env env, Napi::Object exports) {
	Napi::Function func = DefineClass(env, "Player", {
		InstanceMethod("setVideo", &Player::SetVideo),
		InstanceMethod("play", &Player::Play),
		InstanceMethod("pause", &Player::Pause),
		InstanceMethod("setPosition", &Player::SetPosition),
		InstanceMethod("onSize", &Player::OnSize),
		InstanceMethod("onRender", &Player::OnRender)
		});

	exports.Set("Player", func);

	return exports;
}

Player::Player(const Napi::CallbackInfo& info) : Napi::ObjectWrap<Player>(info) {
	this->_libvlc = libvlc_new(0, NULL);

	this->_media = NULL;
	this->_player = NULL;
	this->_readyCB = NULL;
	this->_renderCB = NULL;
	this->_videoMem = NULL;
	this->_playing = false;
}

Napi::Value Player::SetVideo(const Napi::CallbackInfo& info) {
	this->_filePath = info[0].As<Napi::String>().Utf8Value();

	if (this->_media != NULL) {
		libvlc_media_release(this->_media);
	}

	if (this->_player != NULL) {
		libvlc_media_player_release(this->_player);
	}

	this->_media = libvlc_media_new_path(this->_libvlc, this->_filePath.c_str());
	this->_player = libvlc_media_player_new_from_media(this->_media);

	libvlc_video_set_callbacks(this->_player, &Lock, NULL, &Display, this);

	libvlc_event_manager_t* mng = libvlc_media_player_event_manager(this->_player);
	libvlc_event_attach(mng, libvlc_event_e::libvlc_MediaPlayerEndReached, &EndReached, this);

	libvlc_video_set_format_callbacks(this->_player, &VideoFormat, NULL);
	libvlc_audio_set_mute(this->_player, 1);
	std::cout << "Load" << std::endl;

	return Napi::Value::Value();
}

Napi::Value Player::OnSize(const Napi::CallbackInfo& info) {
	Napi::Env env = info.Env();

	this->_readyCB = Napi::ThreadSafeFunction::New(env, info[0].As<Napi::Function>(), "ReadyCb", 0, 1);

	return Napi::Value::Value();
}

Napi::Value Player::OnRender(const Napi::CallbackInfo& info) {
	Napi::Env env = info.Env();

	this->_renderCB = Napi::ThreadSafeFunction::New(env, info[0].As<Napi::Function>(), "RenderCb", 0, 1);

	return Napi::Value::Value();
}

Napi::Value Player::Play(const Napi::CallbackInfo& info) {
	Napi::Env env = info.Env();

	libvlc_time_t length = libvlc_media_get_duration(this->_media);
	libvlc_time_t curr = libvlc_media_player_get_time(this->_player);

	if (length - curr < 500) {
		libvlc_media_player_set_time(this->_player, 0);
	}

	this->_playing = true;
	this->_ended = false;

	libvlc_media_player_play(this->_player);

	return Napi::Value::Value();
}

Napi::Value Player::Pause(const Napi::CallbackInfo& info) {
	Napi::Env env = info.Env();

	this->_playing = false;

	libvlc_media_player_set_pause(this->_player, 1);

	return Napi::Value::Value();
}

Napi::Value Player::SetPosition(const Napi::CallbackInfo& info) {
	Napi::Env env = info.Env();

	libvlc_time_t curr = libvlc_media_player_get_time(this->_player);
	libvlc_time_t length = libvlc_media_get_duration(this->_media);
	libvlc_time_t time = info[0].As<Napi::Number>().Int64Value();

	std::cout << "Ended=" << this->_ended << ", Playing=" << this->_playing << ", Seek=" << time << ", Diff=" << abs(curr - time) << ", length=" << length << std::endl;

	bool ended = this->_ended;
	bool playing = this->_playing;

	if (length - 500 < time) {
		time = length - 500;

		libvlc_media_player_set_pause(this->_player, 1);
	}

	libvlc_media_player_set_time(this->_player, time);

	return Napi::Value::Value();
}

void* Player::Lock(void* opaque, void** planes) {
	Player* player = (Player*)opaque;

	return *planes = player->_videoMem;
}

void Player::Display(void* opaque, void* picture) {
	Player* player = (Player*)opaque;

	auto callback = [](Napi::Env env, Napi::Function jsCallback, void* opaque) {
		Player* player = (Player*)opaque;

		Napi::ArrayBuffer arr = Napi::ArrayBuffer::New(env, player->_videoMem, player->_width * player->_height * 4);

		jsCallback.Call({ Napi::Uint8Array::New(env, player->_width * player->_height * 4, arr, 0, napi_typedarray_type::napi_uint8_clamped_array) });
	};

	libvlc_time_t length = libvlc_media_get_duration(player->_media);
	libvlc_time_t curr = libvlc_media_player_get_time(player->_player);

	if (length - curr < 500) {
		libvlc_media_player_set_pause(player->_player, 1);
	}

	player->_renderCB.BlockingCall(player, callback);
}

unsigned int Player::VideoFormat(void** opaque, char* chroma, unsigned* width, unsigned* height, unsigned* pitches, unsigned* lines)
{
	Player* player = (Player*)*opaque;

	auto callback = [](Napi::Env env, Napi::Function jsCallback, void* opaque) {
		Player* player = (Player*)opaque;

		jsCallback.Call({ Napi::Number::From(env, player->_width), Napi::Number::From(env, player->_height) });
	};

	memcpy(chroma, "RGBA", 4);

	if (*width > 1920) {
		*width = 1920;
	}

	if (*height > 1080) {
		*height = 1080;
	}

	player->_width = *width;
	player->_height = *height;

	if (player->_videoMem != NULL)
		delete player->_videoMem;

	player->_videoMem = new char[player->_width * player->_height * 4];

	*pitches = player->_width * 4;
	*lines = player->_height;

	player->_readyCB.NonBlockingCall(player, callback);

	return 1;
}

void Player::EndReached(const struct libvlc_event_t* p_event, void* p_data) {
	Player* player = (Player*)p_data;

	std::cout << "ENDED!" << std::endl;
	player->_ended = true;
}

Player::~Player()
{
	if (this->_player != NULL) {
		libvlc_media_player_stop(this->_player);
		libvlc_media_player_release(this->_player);
	}

	if (this->_media != NULL)
	{
		libvlc_media_release(this->_media);
	}

	if (this->_videoMem != NULL) {
		delete this->_videoMem;
	}

	libvlc_release(this->_libvlc);

	if (this->_readyCB != NULL) {
		this->_readyCB.Release();
	}

	if (this->_renderCB != NULL) {
		this->_renderCB.Release();
	}

	this->_player = NULL;
	this->_libvlc = NULL;
	this->_readyCB = NULL;
	this->_videoMem = NULL;
	this->_renderCB = NULL;

	std::cout << "RELEASED!" << std::endl;
}
