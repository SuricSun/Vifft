#pragma once

#include"__Util_Common.h"

using namespace std;
using namespace Suancai;

namespace Suancai {

	namespace Util {

		class UTFStringAffair {
		public:
			enum {
				InvalidArg = -1,
				CantConvert = -2
			};
		public:
			/// <summary>
			/// from是待转换字符串，to是结果字符串，确保to里面没有重要内容因为里面的东西会被覆写
			/// </summary>
			/// <param name="from">in</param>
			/// <param name="to">in</param>
			static i64 UTF8To16(const u8string* from, u16string* to);
			/// <summary>
			/// from是待转换字符串，to是结果字符串，确保to里面没有重要内容因为里面的东西会被覆写
			/// </summary>
			/// <param name="from">in</param>
			/// <param name="to">in</param>
			static i64 UTF8To16(const u8string& from, u16string& to);
			/// <summary>
			/// 如果toSizeInByte为0，那么函数返回转换后需要的缓冲区大小，In Bytes，to参数被忽略
			/// </summary>
			/// <param name="from">in</param>
			/// <param name="fromSizeInByte">in</param>
			/// <param name="to">in</param>
			/// <param name="toSizeInByte">in</param>
			/// <returns></returns>
			static i64 UTF8To16(const char8_t* from, i64 fromSizeInByte, char16_t* to, i64 toSizeInByte);
			/// <summary>
			/// from是待转换字符串，to是结果字符串，确保to里面没有重要内容因为里面的东西会被覆写
			/// </summary>
			/// <param name="from">in</param>
			/// <param name="to">in</param>
			static i64 UTF16To8(const u16string* from, u8string* to);
			/// <summary>
			/// from是待转换字符串，to是结果字符串，确保to里面没有重要内容因为里面的东西会被覆写
			/// </summary>
			/// <param name="from">in</param>
			/// <param name="to">in</param>
			static i64 UTF16To8(const u16string& from, u8string& to);
			/// <summary>
			/// 如果toSizeInByte为0，那么函数返回转换后需要的缓冲区大小，In Bytes，to参数被忽略
			/// </summary>
			/// <param name="from">in</param>
			/// <param name="fromSizeInByte">in</param>
			/// <param name="to">in</param>
			/// <param name="toSizeinByte">in</param>
			/// <returns></returns>
			static i64 UTF16To8(const char16_t* from, i64 fromSizeInByte, char8_t* to, i64 toSizeInByte);
			/// <summary>
			/// 返回字符串字符数目不是byte数目
			/// </summary>
			/// <param name="str"></param>
			/// <param name="maxSize">如果是0的话代表一直找</param>
			/// <returns>如果找不到空字符，那么返回0，否则返回长度，包括结尾空字符</returns>
			static i64 UTF8StrLen(const char8_t* str, i64 maxSize);
			/// <summary>
			/// 返回字符串字符数目不是byte数目
			/// </summary>
			/// <param name="str"></param>
			/// <param name="maxSize">如果是0的话代表一直找</param>
			/// <returns>如果找不到空字符，那么返回0，否则返回长度，包括结尾空字符</returns>
			static i64 UTF16StrLen(const char16_t* str, i64 maxSize);
		};
	}
}
