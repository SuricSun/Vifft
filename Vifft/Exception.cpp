#include"Common_Exception.h"

void Suancai::Common_Exception::Base_exception::UTF8To16(const u8string* from, u16string* to) {

	i64 requiredCharCnt = MultiByteToWideChar(CP_UTF8, 0, (CHAR*)from->data(), (int)from->size(), nullptr, 0);
	if (requiredCharCnt == 0) {
		to->assign(u"Cant convert utf8 to utf16");
		return;
	}
	to->resize(requiredCharCnt);
	requiredCharCnt = MultiByteToWideChar(CP_UTF8, 0, (CHAR*)from->data(), (int)from->size(), (WCHAR*)to->data(), (int)to->size());
	if (requiredCharCnt == 0) {
		to->assign(u"Cant convert utf8 to utf16");
		return;
	}
}

void Suancai::Common_Exception::Base_exception::UTF16To8(const u16string* from, u8string* to) {

	i64 requiredByteCnt = WideCharToMultiByte(CP_UTF8, 0, (WCHAR*)from->data(), from->size(), nullptr, 0, nullptr, nullptr);
	if (requiredByteCnt == 0) {
		to->assign(u8"Cant convert utf16 to utf8");
		return;
	}
	to->resize(requiredByteCnt);
	requiredByteCnt = WideCharToMultiByte(CP_UTF8, 0, (WCHAR*)from->data(), from->size(), (CHAR*)to->data(), to->size(), nullptr, nullptr);
	if (requiredByteCnt == 0) {
		to->assign(u8"Cant convert utf16 to utf8");
		return;
	}
}

Suancai::Common_Exception::Base_exception::Base_exception(const char8_t* p_info) {

	this->info.assign(p_info);
}

void Suancai::Common_Exception::Base_exception::showMsg() {

	u16string info = {};
	u16string class_name = {};
	this->UTF8To16(addr(this->class_name), addr(class_name));
	u8string final_content;
	final_content.append(u8"[Info]: ").append(this->info).append(u8"\n");
	final_content.append(u8"[File]: ").append(this->file_name).append(u8"\n");
	if (this->line_number_set) {
		ss.str(string());
		ss << oct << this->line_number;
		final_content.append(u8"[Line]: ").append((char8_t*)ss.str().c_str()).append(u8"\n");
	} else {
		final_content.append(u8"[Line]: ").append(u8"Not provided\n");
	}
	final_content.append(u8"[Func]: ").append(this->func_name).append(u8"\n");
	if (this->error_code_set) {
		ss.str(string());
		ss << "0x" << hex << this->error_code;
		final_content.append(u8"[Code]: ").append((char8_t*)ss.str().c_str());
	} else {
		final_content.append(u8"[Code]: ").append(u8"Not provided");
	}
	u16string u16_ctnt;
	this->UTF8To16(addr(final_content), addr(u16_ctnt));
	MessageBox(nullptr, (WCHAR*)u16_ctnt.c_str(), (WCHAR*)class_name.c_str(), MB_OK | MB_ICONERROR | MB_TOPMOST);
}

Suancai::Common_Exception::Base_exception* Suancai::Common_Exception::Base_exception::with_file_name(const char8_t* p_file_name) {

	this->file_name.assign(p_file_name);
	return this;
}

Suancai::Common_Exception::Base_exception* Suancai::Common_Exception::Base_exception::with_line_number(i64 line_number) {
	
	this->line_number_set = true;
	this->line_number = line_number;
	return this;
}

Suancai::Common_Exception::Base_exception* Suancai::Common_Exception::Base_exception::with_func_name(const char8_t* p_func_name) {
	
	this->func_name.assign(p_func_name);
	return this;
}

Suancai::Common_Exception::Base_exception* Suancai::Common_Exception::Base_exception::with_error_code(i64 error_code) {
	
	this->error_code_set = true;
	this->error_code = error_code;
	return this;
}

Suancai::Common_Exception::Base_exception::~Base_exception() {
}

Suancai::Common_Exception::Invalid_arg_exception::Invalid_arg_exception(const char8_t* p_info) : Base_exception(p_info) {

	this->class_name.assign(u8"Invalid_arg_exception");
}
