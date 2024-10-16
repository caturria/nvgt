//TODO: rename to sound.cpp when complete.
/* sound2.cpp - sound system implementation code
 *
 * NVGT - NonVisual Gaming Toolkit
 * Copyright (c) 2022-2024 Sam Tupy
 * https://nvgt.gg
 * This software is provided "as-is", without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.
 * Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:
 * 1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
*/
#include <string>
#include <vector>
#include <angelscript.h>
#include <scriptarray.h>
#include "sound2.h"
#include "sound_service.h"
using namespace std;
namespace sound2
{

	// Miniaudio objects must be allocated on the heap as nvgt's API introduces the concept of an uninitialized sound, which a stack based system would make more difficult to implement.
	class audio_engine_impl : public audio_engine {
		ma_engine* engine;
		audio_node* engine_endpoint; // Upon engine creation we'll call ma_engine_get_endpoint once so as to avoid creating more than one of our wrapper objects when our engine->get_endpoint() function is called.
	public:
		audio_engine_impl() {
			// default constructor initializes a miniaudio engine ourselves.
			ma_engine_config cfg = ma_engine_config_init();
		}
	};
	class sound_impl : public sound {
		ma_sound* snd;
	};
	CScriptArray* list_playback_devices()
	{
		_sound_service(nullptr)
			return service->get_context()->get_playback_devices();
	}
	unsigned int get_playback_device_count()
	{
		_sound_service(0);
		return service->get_context()->get_playback_device_count();
	}
	CScriptArray* list_capture_devices()
	{
		_sound_service(nullptr);
		return service->get_context()->get_capture_devices();

	}
	unsigned int get_capture_device_count()
	{
		_sound_service(0);
		return service->get_context()->get_capture_device_count();
	}
	void RegisterSoundsystem(asIScriptEngine* engine)
	{
		engine->RegisterGlobalFunction("uint get_s2_sound_output_device_count() property", asFUNCTION(get_playback_device_count), asCALL_CDECL);
		engine->RegisterGlobalFunction("array<string>@ get_s2_sound_output_devices() property", asFUNCTION(list_playback_devices), asCALL_CDECL);
		engine->RegisterGlobalFunction("uint get_s2_sound_input_device_count() property", asFUNCTION(get_capture_device_count), asCALL_CDECL);
		engine->RegisterGlobalFunction("array<string>@ get_s2_sound_input_devices() property", asFUNCTION(list_capture_devices), asCALL_CDECL);

	}
}
