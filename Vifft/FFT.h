#pragma once

#include"__Util_Common.h"

#include<math.h>

#define PI_d 3.1415926535897932384626433832795
#define PI_f 3.1415926535897932384626433832795f

namespace Suancai {

	namespace Util {

		class FFT {
		public:
			struct Complex {
				float real = 0;
				float imagine = 0;
			};
		public:
			u32 fft_size = 0;
			Complex* p_result0 = nullptr;
			Complex* p_result1_final = nullptr;
		protected:
		public:
			FFT();
			void init_buffer(u32 fft_size);
			void do_fft(float* p_samples, u32 samples_offset, u32 sample_size, u32 sample_start, u32 sample_step, u32 sample_cnt);
			void do_fft_inner(float* samples, u32 samples_offset, u32 sample_length, u32 sample_start, u32 sample_step, u32 sample_cnt, Complex* p_buffer_read, Complex* p_buffer_write);
			~FFT();
		};
	}
}