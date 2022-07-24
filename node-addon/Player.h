#pragma once

#if defined(_MSC_VER)
#include <BaseTsd.h>
typedef SSIZE_T ssize_t;
#endif

#include<mutex>
#include <napi.h>
#include <vlc/vlc.h>

class Player : public Napi::ObjectWrap<Player> {
public:
	static Napi::Object Player::Init(Napi::Env, Napi::Object exports);
	Player(const Napi::CallbackInfo& info);
	~Player();

private:
	Napi::Value Play(const Napi::CallbackInfo& info);
	Napi::Value SetVideo(const Napi::CallbackInfo& info);
	Napi::Value OnSize(const Napi::CallbackInfo& info);
	Napi::Value OnRender(const Napi::CallbackInfo& info);
	Napi::Value SetPosition(const Napi::CallbackInfo& info);
	Napi::Value Pause(const Napi::CallbackInfo& info);

	static void* Player::Lock(void* opaque, void** planes);
	static void Player::Display(void* opaque, void* picture);
	static void Player::EndReached(const struct libvlc_event_t* p_event, void* p_data);
	static unsigned int Player::VideoFormat(void** opaque, char* chroma, unsigned* width, unsigned* height, unsigned* pitches, unsigned* lines);

	uint32_t _width;
	uint32_t _height;

	std::string _filePath;

	bool _ended;
	bool _playing;
	bool _restart;
	char* _videoMem;

	libvlc_media_t* _media;
	libvlc_instance_t* _libvlc;
	libvlc_media_player_t* _player;

	Napi::ThreadSafeFunction _readyCB;
	Napi::ThreadSafeFunction _renderCB;
};