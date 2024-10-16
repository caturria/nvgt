/* sound_service.h - Global sound state management and service container for sound related objects 
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
#pragma once
#include <scriptarray.h>
#include <miniaudio.h>//ma_result
#include <exception>
using std::exception;
#define _sound_service(x) \
sound_service* service = sound_service::get_instance();\
if (service == nullptr)\
return x;

namespace sound2//for now
{
//An exception which contains a ma_result code (and potentially other useful information later).
	class ma_exception: public exception
	{
		ma_result code;
	public:
		ma_exception(ma_result code);
		ma_result get_code();
};
	class sound_service;
	//Basic functionality that's common to all objects that deal with Miniaudio.
	class sound_base
	{
	protected:
		sound_service* service;
	public:
		sound_base(sound_service* service);
		virtual ~sound_base();
		//Attempt a miniaudio operation that returns a ma_result. If it fails, this automatically handles updating the last error with the sound_service and bailing out.
		ma_result attempt(ma_result result, bool throw_ma_exception = false);

	};
	//An audio context for device management.
	class sound_context: public sound_base
	{
	public:
		sound_context(sound_service* service);
		virtual ~sound_context();
		virtual CScriptArray* get_capture_devices() = 0;
		virtual CScriptArray* get_playback_devices() = 0;
		virtual unsigned int get_playback_device_count() = 0;
		virtual unsigned int get_capture_device_count() = 0;

	};
	class sound_service
	{
		static sound_service* instance;	
	public:
		virtual ~sound_service();

		//Returns a pointer to the sound service. If this returns non-null you can be sure that the sound service is fully initialized.
		static sound_service* get_instance();
//Destroys the sound service. It can be recreated just by calling get_instance again.
 		   static void destroy();
//Sets (and returns) a ma_result error code.
		virtual ma_result set_last_error(ma_result code) = 0;
		virtual ma_result get_last_error() = 0;
//Gets a pointer to the device context.
		virtual sound_context* get_context() = 0;

	};
}
