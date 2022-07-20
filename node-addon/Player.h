#pragma once

#include<mutex>
#include <napi.h>
#include <vlc/vlc.h>

class Player : public Napi::ObjectWrap<Player> {
public:
	static Napi::Object Player::Init(Napi::Env, Napi::Object exports);
	Player(const Napi::CallbackInfo& info);

private:
	Napi::Value Play(const Napi::CallbackInfo& info);
	Napi::Value SetVideo(const Napi::CallbackInfo& info);
	Napi::Value SetDimensions(const Napi::CallbackInfo& info);
	Napi::Value SetRenderCallback(const Napi::CallbackInfo& info);

	static void* Player::Lock(void* opaque, void** planes);
	static void Player::Display(void* opaque, void* picture);
	
	uint32_t _width;
	uint32_t _height;

	std::string _filePath;

	char* _frame;
	bool _playing;
	char* _videoMem;
	std::mutex _videoMemMutex;
	libvlc_media_t* _media;
	libvlc_instance_t* _libvlc;
	libvlc_media_player_t* _player;

	Napi::ThreadSafeFunction _renderCB;
};