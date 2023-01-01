#include"UTFString_Affair.h"

using namespace Suancai;

i64 Suancai::Util::UTFStringAffair::UTF8To16(const u8string* from, u16string* to) {

	if (from == nullptr || to == nullptr) {
		return InvalidArg;
	}
	i64 requiredCharCnt = MultiByteToWideChar(CP_UTF8, 0, (CHAR*)from->data(), from->size(), nullptr, 0);
	if (requiredCharCnt == 0) {
		return CantConvert;
	}
	to->resize(requiredCharCnt);
	requiredCharCnt = MultiByteToWideChar(CP_UTF8, 0, (CHAR*)from->data(), from->size(), (WCHAR*)to->data(), to->size());
	if (requiredCharCnt == 0) {
		return CantConvert;
	}

	return 0;
}

i64 Suancai::Util::UTFStringAffair::UTF8To16(const u8string& from, u16string& to) {

	return UTFStringAffair::UTF8To16(addr(from), addr(to));
}

i64 Suancai::Util::UTFStringAffair::UTF16To8(const u16string* from, u8string* to) {
	
	if (from == nullptr || to == nullptr) {
		return InvalidArg;
	}
	i64 requiredByteCnt = WideCharToMultiByte(CP_UTF8, 0, (WCHAR*)from->data(), from->size(), nullptr, 0, nullptr, nullptr);
	if (requiredByteCnt == 0) {
		return CantConvert;
	}
	to->resize(requiredByteCnt);
	requiredByteCnt = WideCharToMultiByte(CP_UTF8, 0, (WCHAR*)from->data(), from->size(), (CHAR*)to->data(), to->size(), nullptr, nullptr);
	if (requiredByteCnt == 0) {
		return CantConvert;
	}

	return 0;
}

i64 Suancai::Util::UTFStringAffair::UTF16To8(const u16string& from, u8string& to) {

	return UTFStringAffair::UTF16To8(addr(from), addr(to));
}

i64 Suancai::Util::UTFStringAffair::UTF8To16(const char8_t* from, i64 fromSizeInByte, char16_t* to, i64 toSizeInByte) {

	if (from == nullptr) {
		return InvalidArg;
	}
	if (toSizeInByte == 0) {
		i64 require_size = MultiByteToWideChar(CP_UTF8, 0, (LPCCH)from, fromSizeInByte, nullptr, 0);
		if (require_size == 0) {
			return CantConvert;
		}
		return require_size * 2;
	}
	if (to == nullptr) {
		return InvalidArg;
	}
	if (MultiByteToWideChar(CP_UTF8, 0, (LPCCH)from, fromSizeInByte, (LPWSTR)to, toSizeInByte / 2) == 0) {
		return CantConvert;
	}
	return 0;
}

i64 Suancai::Util::UTFStringAffair::UTF16To8(const char16_t* from, i64 fromSizeInByte, char8_t* to, i64 toSizeInByte) {

	if (from == nullptr) {
		return InvalidArg;
	}
	if (toSizeInByte == 0) {
		i64 require_cnt = WideCharToMultiByte(CP_UTF8, 0, (LPCWCH)from, fromSizeInByte / 2, nullptr, 0, nullptr, nullptr);
		if (require_cnt == 0) {
			return CantConvert;
		}
		return require_cnt;
	}
	if (to == nullptr) {
		return InvalidArg;
	}
	if (WideCharToMultiByte(CP_UTF8, 0, (LPCWCH)from, fromSizeInByte / 2, (LPSTR)to, toSizeInByte, nullptr, nullptr) == 0) {
		return CantConvert;
	}
	return 0;
}

i64 Suancai::Util::UTFStringAffair::UTF8StrLen(const char8_t* str, i64 maxSize) {

	if (maxSize == 0) {
		maxSize = 0x7FFFFFFFFFFFFFFF;
	}
	for (i64 i = 0; i < maxSize; i++) {
		if (str[i] == u8'\0') {
			return i + 1;
		}
	}
	return 0;
}

i64 Suancai::Util::UTFStringAffair::UTF16StrLen(const char16_t* str, i64 maxSize) {

	if (maxSize == 0) {
		maxSize = 0x7FFFFFFFFFFFFFFF;
	}
	for (i64 i = 0; i < maxSize; i++) {
		if (str[i] == u'\0') {
			return i + 1;
		}
	}
	return 0;
}
