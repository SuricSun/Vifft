#include "__Audio_Common.h"

Suancai::Media::Audio::Audio_Exception::Audio_Exception(const char8_t* p_info) : Suancai::Common_Exception::Base_exception(p_info) {

	this->class_name.assign(p_info);
}
