#pragma once

#include"__Media_Common.h"

#include"Common_Exception.h"

namespace Suancai {

	namespace Media {

		namespace Audio {

			enum class Audio_sample_Format : u16 {
				Undefined,
				PCM,
				IEEE_Float
			};

			class Audio_Exception : public Suancai::Common_Exception::Base_exception {
			public:
				Audio_Exception(const char8_t* p_info);
			};
		}
	}
}