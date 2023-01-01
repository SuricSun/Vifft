#pragma once

#include"__Audio_Common.h"

namespace Suancai {

	namespace Media {

		namespace Audio {

			using namespace Microsoft::WRL;
			using namespace std;

			class Audio_Enumerator {
			public:
				IMMDeviceEnumerator* p_enumerator = nullptr;
				vector<ComPtr<IMMDevice>> p_renderDeviceVec;
				vector<ComPtr<IMMDevice>> p_captureDeviceVec;
			public:
				Audio_Enumerator();
				void init();
				void enum_audio_device();
				IMMDevice* get_default_render_device();
				IMMDevice* get_default_capture_device();
				~Audio_Enumerator();
			};
		}
	}
}