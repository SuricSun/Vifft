#pragma once

#include"__Util_Common.h"

namespace Suancai {

	namespace Util {

		class alignas(8) Simple_Mutex {
		protected:
			u32 cur_thread_id = 0;
			void* p_locked_data = nullptr;
		public:
			Simple_Mutex();
			Simple_Mutex(void* p_initial_locked_data);
			bool is_locked();
			bool is_owner();
			bool try_get();
			bool release();
			bool try_get_With_locked_data(void** pp_data);
			bool release_With_locked_data(void* p_data_to_lock);
			~Simple_Mutex();
		};
	}
}
