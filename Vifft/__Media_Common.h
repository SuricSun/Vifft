#pragma once

#include"__Suancai_Common.h"

#include"Common_Exception.h"

#include<mmreg.h>
#include<wtypes.h>
#include<propkeydef.h>
#include<functiondiscoverykeys_devpkey.h>

#include<mmdeviceapi.h>
#include<audioendpoints.h>
#include<audioclient.h>

//include comptr
#include<wrl/client.h>

#include<vector>
#include<string>
#include<map>
#include<set>

#define THROW_AUD(info, error_code) SUANCAI_THROW(info, error_code, Suancai::Media::Audio::Audio_Exception)
#define THROW_ON_CONDITION_AUD(boolExpr, info, error_code) if(boolExpr){SUANCAI_THROW(info, error_code, Suancai::Media::Audio::Audio_Exception);}
#define THROW_ON_HR_FAILED_AUD(hr, info, error_code) if(hr!=S_OK){SUANCAI_THROW(info, error_code, Suancai::Media::Audio::Audio_Exception);}
#define THROW_ON_NULL_AUD(arg, info, error_code) if(arg==NULL){SUANCAI_THROW(info, error_code, Suancai::Media::Audio::Audio_Exception);}

namespace Suancai {

	namespace Media {

	
	}
}