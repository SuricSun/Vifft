#pragma once

#include"__Suancai_Common.h"

#include<string>
#include<sstream>

using namespace std;

namespace Suancai {

	namespace Common_Exception {
		
		using namespace std;

		class Base_exception {
		public:
			stringstream ss;

			u8string class_name = u8"Base_exception";
			u8string info = u8"not provided";
			u8string file_name = u8"not provided";
			u8string func_name = u8"not provided";
			bool line_number_set = false;
			i64 line_number = 0;
			bool error_code_set = false;
			i64 error_code = 0;
		public:
			static void UTF8To16(const u8string* from, u16string* to);
			static void UTF16To8(const u16string* from, u8string* to);
		public:
			Base_exception(const char8_t* p_info);
			void showMsg();
			Base_exception* with_file_name(const char8_t* p_file_name);
			Base_exception* with_line_number(i64 line_number);
			Base_exception* with_func_name(const char8_t* p_func_name);
			Base_exception* with_error_code(i64 error_code);
			~Base_exception();
		};

		class Invalid_arg_exception : public Base_exception {
		public:
			//继承父类构造函数
			Invalid_arg_exception(const char8_t* p_info);
		};
	}
}
