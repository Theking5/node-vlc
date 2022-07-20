#define NAPI_CPP_EXCEPTIONS

#include <napi.h>
#include <vlc/vlc.h>
#include <Windows.h>

libvlc_media_t* media;
libvlc_instance_t* libvlc;
libvlc_media_player_t* player;
Napi::HandleScope* scope;

struct Data {
	void* mem;
	Napi::Env env;
	Napi::ThreadSafeFunction callback;


	Data(const Napi::CallbackInfo& info, Napi::ThreadSafeFunction callback, int size) : env(info.Env()), callback(callback), mem(new char[size]) {}

	~Data() {
		delete mem;
	}
};

void* lock(void* opaque, void** planes);
void display(void* opaque, void* picture);

Napi::Value Play(const Napi::CallbackInfo& info) {
	Napi::Env env = info.Env();

	Napi::ThreadSafeFunction f = Napi::ThreadSafeFunction::New(
		env,
		info[0].As<Napi::Function>(),  // JavaScript function called asynchronously
		"Resource Name",         // Name
		0,                       // Unlimited queue
		1);

    media = libvlc_media_new_path(libvlc, "C:\\Users\\David\\Downloads\\mov_bbb.mp4");
	player = libvlc_media_player_new_from_media(media);

	Data* data = new Data(info,f, 1920*1080*4);

	libvlc_video_set_format(player, "RGBA", 1920, 1080, 1920 * 4);
	libvlc_video_set_callbacks(player, &lock, NULL, &display, data);
	libvlc_media_player_play(player);

	return Napi::Value::Value();
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
	libvlc = libvlc_new(0, NULL);

	exports.Set(Napi::String::New(env, "play"), Napi::Function::New(env, Play));
	return exports;
}

void* lock(void* opaque, void** planes) {
	Data* d = (Data*)opaque;

	*planes = d->mem;

	return d->mem;
}

void display(void* opaque, void* picture) {
	Data* d = (Data*)opaque;

	auto callback = [](Napi::Env env, Napi::Function jsCallback, void* value) {
		Napi::ArrayBuffer arr = Napi::ArrayBuffer::New(env, value, 1920 * 1080 * 4);

    	jsCallback.Call({ Napi::Uint8Array::New(env, 1920 * 1080 * 4, arr, 0, napi_typedarray_type::napi_uint8_clamped_array) });
		//delete value;
	};

	char* dd = new char[1920*1080*4];
	memcpy(dd, picture, 1920 * 1080*4);

   d->callback.BlockingCall(dd, callback);

}


NODE_API_MODULE(hello, Init)