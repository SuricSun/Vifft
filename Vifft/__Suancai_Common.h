#pragma once

/*
* So what about cpp as java?
* Cuz cpp lib sucks.
*
* Days later me: Sry about that cpp lib good
*
* Days later me: No cpp lib still sucks
*/

/**
 *                             _ooOoo_
 *                            o8888888o
 *                            88" . "88
 *                            (| -_- |)
 *                            O\  =  /O
 *                         ____/`---'\____
 *                       .'  \\|     |//  `.
 *                      /  \\|||  :  |||//  \
 *                     /  _||||| -:- |||||-  \
 *                     |   | \\\  -  /// |   |
 *                     | \_|  ''\---/''  |   |
 *                     \  .-\__  `-`  ___/-. /
 *                   ___`. .'  /--.--\  `. . __
 *                ."" '<  `.___\_<|>_/___.'  >'"".
 *               | | :  `- \`.;`\ _ /`;.`/ - ` : | |
 *               \  \ `-.   \_ __\ /__ _/   .-` /  /
 *          ======`-.____`-.___\_____/___.-`____.-'======
 *                             `=---='
 *          ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
*/

#include<stdint.h>
#include<Windows.h>

#include<assert.h>

#define ___DEL___

//delete
#define del(arg) delete (arg);

//delete[]
#define dela(arg) delete[] (arg);

//delete with safe check
#define sdel(arg) if((arg)!=nullptr){delete (arg);}

//delete[] with safe check
#define sdela(arg) if((arg)!=nullptr){delete[] (arg);}

//set null
#define nul(arg) (arg)=nullptr;

//addr of
#define addr(arg) (&(arg))

//deref
#define deref(arg) (*(arg))

//com
#define Safe_Release(arg) if((arg)!=nullptr){(arg)->Release();}

#define MAX_U8  0xFFu
#define MAX_U16 0xFFFFu
#define MAX_U32 0xFFFFFFFFu
#define MAX_U64 0xFFFFFFFFFFFFFFFFu

#define SUANCAI_THROW(info, error_code, exception_name) \
u16string file_name = u16string((const char16_t*)__FILEW__); \
u8string u8_file_name; \
Suancai::Common_Exception::Base_exception::UTF16To8(addr(file_name), addr(u8_file_name)); \
throw (new exception_name(u8#info))->with_file_name((const char8_t*)u8_file_name.c_str())->with_line_number(__LINE__)->with_error_code(error_code);

namespace Suancai {

	using i8 = int8_t;
	using i16 = int16_t;
	using i32 = int32_t;
	using i64 = int64_t;

	using u8 = uint8_t;
	using u16 = uint16_t;
	using u32 = uint32_t;
	using u64 = uint64_t;
}

