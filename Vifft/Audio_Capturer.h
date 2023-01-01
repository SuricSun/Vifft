#pragma once

#include"__Audio_Common.h"

#include"UTFString_Affair.h"

namespace Suancai {

	namespace Media {

		namespace Audio {

			using namespace Suancai::Util;

			class Audio_Capturer {
			public:
				IMMDevice* p_device = nullptr;
				IAudioClient* p_client = nullptr;
				IAudioCaptureClient* p_capture_client = nullptr;

				WAVEFORMATEXTENSIBLE* p_wave_fmt = nullptr;
				Audio_sample_Format audio_fmt = {};

				u32 sample_bit_depth = 0;
				u32 channels = 0;
				u32 frameSizeInByte = 0;
				u32 samples_per_sec = 0;
				//AudioCaptureClient内部的捕获缓冲区实际大小
				u32 buffer_frame_cnt = 0;
				//name
				u16string device_name;
			public:
				Audio_Capturer();
				/// <summary>
				/// 
				/// </summary>
				/// <param name="p_audDevice"></param>
				/// <param name="bufferDurationIn100NanoUnit">
				/// 10000000 means 1 sec
				/// which means alloc a buffer that contains 1 sec of audio data, buffer byte size is according to the current audio device output format
				/// </param>
				/// <param name="whetherLoopbackCapturing">if set to true, means capture stream from a render device, so p_audDevice should not be a capture device such as a microphone</param>
				void init(IMMDevice* p_audDevice, u32 bufferDurationIn100NanoUnit, bool whetherLoopbackCapturing);
				/// <summary>
				/// tell the system to start capture audio data, you can aquire the data from AudioCaptureClient
				/// </summary>
				void enable_capture();
				/// <summary>
				/// tell the system to stop capturing data
				/// </summary>
				void disable_capture();
				IAudioCaptureClient* get_capture_client();
				~Audio_Capturer();
			};
		}
	}
}