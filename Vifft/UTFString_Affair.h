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
			/// from�Ǵ�ת���ַ�����to�ǽ���ַ�����ȷ��to����û����Ҫ������Ϊ����Ķ����ᱻ��д
			/// </summary>
			/// <param name="from">in</param>
			/// <param name="to">in</param>
			static i64 UTF8To16(const u8string* from, u16string* to);
			/// <summary>
			/// from�Ǵ�ת���ַ�����to�ǽ���ַ�����ȷ��to����û����Ҫ������Ϊ����Ķ����ᱻ��д
			/// </summary>
			/// <param name="from">in</param>
			/// <param name="to">in</param>
			static i64 UTF8To16(const u8string& from, u16string& to);
			/// <summary>
			/// ���toSizeInByteΪ0����ô��������ת������Ҫ�Ļ�������С��In Bytes��to����������
			/// </summary>
			/// <param name="from">in</param>
			/// <param name="fromSizeInByte">in</param>
			/// <param name="to">in</param>
			/// <param name="toSizeInByte">in</param>
			/// <returns></returns>
			static i64 UTF8To16(const char8_t* from, i64 fromSizeInByte, char16_t* to, i64 toSizeInByte);
			/// <summary>
			/// from�Ǵ�ת���ַ�����to�ǽ���ַ�����ȷ��to����û����Ҫ������Ϊ����Ķ����ᱻ��д
			/// </summary>
			/// <param name="from">in</param>
			/// <param name="to">in</param>
			static i64 UTF16To8(const u16string* from, u8string* to);
			/// <summary>
			/// from�Ǵ�ת���ַ�����to�ǽ���ַ�����ȷ��to����û����Ҫ������Ϊ����Ķ����ᱻ��д
			/// </summary>
			/// <param name="from">in</param>
			/// <param name="to">in</param>
			static i64 UTF16To8(const u16string& from, u8string& to);
			/// <summary>
			/// ���toSizeInByteΪ0����ô��������ת������Ҫ�Ļ�������С��In Bytes��to����������
			/// </summary>
			/// <param name="from">in</param>
			/// <param name="fromSizeInByte">in</param>
			/// <param name="to">in</param>
			/// <param name="toSizeinByte">in</param>
			/// <returns></returns>
			static i64 UTF16To8(const char16_t* from, i64 fromSizeInByte, char8_t* to, i64 toSizeInByte);
			/// <summary>
			/// �����ַ����ַ���Ŀ����byte��Ŀ
			/// </summary>
			/// <param name="str"></param>
			/// <param name="maxSize">�����0�Ļ�����һֱ��</param>
			/// <returns>����Ҳ������ַ�����ô����0�����򷵻س��ȣ�������β���ַ�</returns>
			static i64 UTF8StrLen(const char8_t* str, i64 maxSize);
			/// <summary>
			/// �����ַ����ַ���Ŀ����byte��Ŀ
			/// </summary>
			/// <param name="str"></param>
			/// <param name="maxSize">�����0�Ļ�����һֱ��</param>
			/// <returns>����Ҳ������ַ�����ô����0�����򷵻س��ȣ�������β���ַ�</returns>
			static i64 UTF16StrLen(const char16_t* str, i64 maxSize);
		};
	}
}
