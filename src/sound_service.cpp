/* sound_service.cpp - Sound service container implementation
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

#include "sound_service.h"
#include <angelscript.h>
#include <scriptarray.h>
#include "sound2.h"
#include <vector>
#include <memory>//unique_ptr.
namespace sound2//for now
{
	class sound_context_impl : public sound_context
	{
		ma_context context;
		std::vector <ma_device_info> playback_devices, capture_devices;
		ma_uint32 playback_device_count, capture_device_count;
		CScriptArray* script_capture_devices = nullptr, *script_playback_devices = nullptr;
//Called during init to prepare the lists of input and output devices.
		void build_device_lists()
		{
			ma_device_info* playback_infos, * capture_infos;
			attempt(ma_context_get_devices(&context, &playback_infos, &playback_device_count, &capture_infos, &capture_device_count), true);
			asIScriptContext* context = asGetActiveContext();
			asIScriptEngine* engine = context->GetEngine();
			asITypeInfo* string_array_type = engine->GetTypeInfoByDecl("array <string>");
			try
			{
				script_playback_devices = CScriptArray::Create(string_array_type, playback_device_count);
				script_capture_devices = CScriptArray::Create(string_array_type, capture_device_count);

				for (int i = 0; i < playback_device_count; i++)
				{
					std::string name(playback_infos[i].name);
					playback_devices.push_back(playback_infos[i]);
					script_playback_devices->InsertLast(&name);
				}
				for (int i = 0; i < capture_device_count; i++)
				{
					std::string name(capture_infos[i].name);
					capture_devices.push_back(playback_infos[i]);
					script_capture_devices->InsertLast(&name);
				}
			}
			catch (exception e)
			{
				if (script_playback_devices != nullptr)
					script_playback_devices->Release();
				if (script_capture_devices != nullptr)
					script_playback_devices->Release();
				throw e;
			}
		}
	public:
		//Because this is initialized during sound_service initialization, it requires a pointer to the sound_service currently being constructed.
		sound_context_impl(sound_service* service)
			:sound_context(service)
		{
			this->service = service;
			try
			{
				ma_context_config config = ma_context_config_init();
				attempt(ma_context_init(NULL, 0, &config, &context), true);
				build_device_lists();
			}
			catch (exception e)
			{
				ma_context_uninit(&context);
				throw e;
			}
			}
		virtual ~sound_context_impl()
		{
			ma_context_uninit(&context);
		}
		CScriptArray* get_capture_devices()
		{
			return script_capture_devices;
		}
		unsigned int get_capture_device_count()
		{
			return (unsigned int) capture_devices.size();
		}
		CScriptArray* get_playback_devices()
		{
			return script_playback_devices;
		}
		unsigned int get_playback_device_count()
		{
			return playback_device_count;
		}
};
	class sound_service_impl : public sound_service
	{
		std::unique_ptr <sound_context> context;
		std::unique_ptr <audio_engine> engine;
		ma_result last_error = MA_SUCCESS;
	public:
		sound_service_impl()
			:sound_service()
		{
			context = std::make_unique <sound_context_impl>(this);
}
		virtual ~sound_service_impl()
		{

		}
		ma_result set_last_error(ma_result result)
		{
			last_error = result;
			return result;
}
		ma_result get_last_error()
		{
			return last_error;
}
		sound_context* get_context()
		{
			return context.get();
}
	};
	ma_exception::ma_exception(ma_result code)
		:exception()
	{
		this->code = code;

}
	ma_result ma_exception::get_code()
	{
		return code;

	}
	sound_base::sound_base(sound_service* service)
	{
		this->service = service;
	}
	sound_context::sound_context(sound_service* service)
		:sound_base(service)
	{

}
	ma_result sound_base::attempt(ma_result result, bool throw_ma_exception)
	{
		if (result != MA_SUCCESS)
		{
			service->set_last_error(result);
			if (throw_ma_exception)
				throw ma_exception(result);
			return result;
		}
		return result;
	}
	sound_service* sound_service::get_instance()
	{
		if (instance != nullptr)
			return instance;
		try
		{
			instance = new sound_service_impl();
			return instance;
}
		catch (...)
		{
			return nullptr;
		}
	}
	void sound_service::destroy()
	{
		delete instance;
	}
	sound_service* sound_service::instance;
	sound_base::~sound_base() {}
	sound_context::~sound_context() {}
	sound_service::~sound_service() {}

}
