#include "Audio_Capturer.h"

Suancai::Media::Audio::Audio_Capturer::Audio_Capturer() {
}

void Suancai::Media::Audio::Audio_Capturer::init(IMMDevice* p_audDevice, u32 bufferDurationIn100NanoUnit, bool whetherLoopbackCapturing) {


	Safe_Release(this->p_client);
	Safe_Release(this->p_device);
	Safe_Release(this->p_capture_client);
	if (this->p_wave_fmt != nullptr) {
		CoTaskMemFree(this->p_wave_fmt);
	}
	//get aud client from device
	this->p_device = p_audDevice;
	//不是自己创建的object要addRef
	this->p_device->AddRef();

	HRESULT hr = this->p_device->Activate(__uuidof(IAudioClient), CLSCTX_ALL, nullptr, (void**)addr(this->p_client));
	THROW_ON_HR_FAILED_AUD(hr, "can't activate p_device", -1);

	hr = this->p_client->GetMixFormat((WAVEFORMATEX**)addr(this->p_wave_fmt));
	THROW_ON_HR_FAILED_AUD(hr, "can't get mix format on aud client", -1);

	if (this->p_wave_fmt->SubFormat == KSDATAFORMAT_SUBTYPE_IEEE_FLOAT) {
		this->audio_fmt = Audio_sample_Format::IEEE_Float;
	} else if (this->p_wave_fmt->SubFormat == KSDATAFORMAT_SUBTYPE_PCM) {
		this->audio_fmt = Audio_sample_Format::PCM;
	} else {
		THROW_AUD("Unsupported device output fmt", -1);
	}

	this->samples_per_sec = this->p_wave_fmt->Format.nSamplesPerSec;
	this->channels = this->p_wave_fmt->Format.nChannels;
	this->sample_bit_depth = this->p_wave_fmt->Format.wBitsPerSample;

	this->frameSizeInByte = this->channels * (this->sample_bit_depth / 8);

	REFERENCE_TIME bufferDuration = bufferDurationIn100NanoUnit;

	DWORD initFlags = 0;
	//active loopback
	if (whetherLoopbackCapturing) {
		initFlags |= AUDCLNT_STREAMFLAGS_LOOPBACK;
	}

	hr =
		this->p_client->Initialize(
			AUDCLNT_SHAREMODE::AUDCLNT_SHAREMODE_SHARED,
			initFlags,
			bufferDuration,
			0,
			(WAVEFORMATEX*)this->p_wave_fmt,
			nullptr
		);
	THROW_ON_HR_FAILED_AUD(hr, "can't initialize p_client", -1);

	//this->hAudioBufferReadyEvent = CreateEvent(nullptr, false, false, nullptr);
	//THROW_ON_NULL_AUD(this->hAudioBufferReadyEvent, "CreateEvent(nullptr, false, false, nullptr)");

	//hr = this->p_client->SetEventHandle(this->hAudioBufferReadyEvent);
	//THROW_ON_HR_FAILED_AUD(hr, "this->p_client->SetEventHandle(this->hAudioBufferReadyEvent)");

	hr = this->p_client->GetBufferSize(addr(this->buffer_frame_cnt));
	THROW_ON_HR_FAILED_AUD(hr, "this->p_client->GetBufferSize(addr(audioFrameCnt))", -1);

	hr = this->p_client->GetService(__uuidof(IAudioCaptureClient), (void**)addr(this->p_capture_client));
	THROW_ON_HR_FAILED_AUD(hr, "this->p_client->GetService(__uuidof(IAudioRenderClient), (void**)addr(this->p_captureClient))", -1);

	IPropertyStore* p_store = NULL;
	this->p_device->OpenPropertyStore(STGM_READ, addr(p_store));
	PROPVARIANT prop = {};
	p_store->GetValue(PKEY_Device_FriendlyName, addr(prop));
	this->device_name.assign((char16_t*)prop.pwszVal);
}

void Suancai::Media::Audio::Audio_Capturer::enable_capture() {

	THROW_ON_HR_FAILED_AUD(this->p_client->Start(), "无法开始捕获", -1);
}

void Suancai::Media::Audio::Audio_Capturer::disable_capture() {

	THROW_ON_HR_FAILED_AUD(this->p_client->Stop(), "无法停止捕获", -1);
}

IAudioCaptureClient* Suancai::Media::Audio::Audio_Capturer::get_capture_client() {
    
	return this->p_capture_client;
}

Suancai::Media::Audio::Audio_Capturer::~Audio_Capturer() {

	Safe_Release(this->p_client);
	Safe_Release(this->p_device);
	Safe_Release(this->p_capture_client);
	if (this->p_wave_fmt != nullptr) {
		CoTaskMemFree(this->p_wave_fmt);
	}
}
