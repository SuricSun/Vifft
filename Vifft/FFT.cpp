#include "FFT.h"

Suancai::Util::FFT::FFT() {
}

void Suancai::Util::FFT::init_buffer(u32 fft_size) {

	if (fft_size % 2 != 0 || fft_size < 1) {
		SUANCAI_THROW("fft_size参数错误", -1, Suancai::Common_Exception::Invalid_arg_exception);
	}
	sdela(this->p_result0);
	sdela(this->p_result1_final);
	this->fft_size = fft_size;
	this->p_result0 = new Complex[this->fft_size];
	this->p_result1_final = new Complex[this->fft_size];
}

void Suancai::Util::FFT::do_fft(float* p_samples, u32 samples_offset, u32 samples_size, u32 sample_start, u32 sample_step, u32 sample_cnt) {
	
	if (samples_size % 2 != 0 || samples_size < 1) {
		SUANCAI_THROW("fft_size参数错误", -1, Suancai::Common_Exception::Invalid_arg_exception);
	}
	this->do_fft_inner(p_samples, samples_offset, samples_size, sample_start, sample_step, sample_cnt, this->p_result0, this->p_result1_final);
	float real = 0.0f, imagine = 0.0f;
	Complex* p_cplx = nullptr;
	for (u32 i = 0; i < sample_cnt; i++) {
		p_cplx = addr(this->p_result1_final[sample_start + sample_step * i]);
		real = sqrtf(powf(p_cplx->real, 2) + powf(p_cplx->imagine, 2));
		imagine = atan2f(p_cplx->imagine, p_cplx->real);
		p_cplx->real = real;
		p_cplx->imagine = imagine;
	}
}

void Suancai::Util::FFT::do_fft_inner(float* p_samples, u32 samples_offset, u32 samples_size, u32 sample_start, u32 sample_step, u32 sample_cnt, Complex* p_read_from, Complex* p_write_to) {

	if (sample_cnt == 1) {
		p_write_to[sample_start].real = p_samples[(samples_offset + sample_start) % samples_size];
		p_write_to[sample_start].imagine = 0;
		return;
	}

	//even
	this->do_fft_inner(p_samples, samples_offset, samples_size, sample_start, sample_step * 2, sample_cnt / 2, p_write_to, p_read_from);
	//odd
	this->do_fft_inner(p_samples, samples_offset, samples_size, sample_start + sample_step, sample_step * 2, sample_cnt / 2, p_write_to, p_read_from);

	//给上级准备数据
	u32 mid = sample_start + sample_step * sample_cnt / 2;
	u32 max_inclusive_idx = sample_start + sample_step * (sample_cnt - 1);
	u32 sample_cnt_mid = sample_cnt / 2;
	float sample_cnt_f = (float)sample_cnt;
	float tmp = 0.0f;
	Complex* p_write_cplx;
	Complex* p_read_cplx;
	float real = 0.0f, imagine = 0.0f;
	for (u32 i = 0; i < sample_cnt_mid; i++) {
		p_write_cplx = addr(p_write_to[sample_start + sample_step * i]);
		p_write_cplx->real = p_write_cplx->imagine = 0;
		tmp = (float)i / sample_cnt_f;
		//欧拉公式
		p_write_cplx->real = cosf(-2 * PI_f * tmp);
		p_write_cplx->imagine = sinf(-2 * PI_f * tmp);
		//乘法
		p_read_cplx = addr(p_read_from[sample_start + sample_step * (2 * i + 1)]);
		real = p_write_cplx->real * p_read_cplx->real - p_write_cplx->imagine * p_read_cplx->imagine;
		imagine = p_write_cplx->real * p_read_cplx->imagine + p_write_cplx->imagine * p_read_cplx->real;
		p_write_cplx->real = real;
		p_write_cplx->imagine = imagine;
		//加法
		p_read_cplx = addr(p_read_from[sample_start + sample_step * 2 * i]);
		p_write_cplx->real += p_read_cplx->real;
		p_write_cplx->imagine += p_read_cplx->imagine;
		//除二
		p_write_cplx->real /= 2.0f;
		p_write_cplx->imagine /= 2.0f;
		/*
		* 分割线
		*/
		p_write_cplx = addr(p_write_to[mid + sample_step * i]);
		p_write_cplx->real = p_write_cplx->imagine = 0;
		//欧拉公式
		p_write_cplx->real = cosf(-2 * PI_f * (tmp + 0.5f));
		p_write_cplx->imagine = sinf(-2 * PI_f * (tmp + 0.5f));
		//乘法
		p_read_cplx = addr(p_read_from[sample_start + sample_step * (2 * i + 1)]);
		real = p_write_cplx->real * p_read_cplx->real - p_write_cplx->imagine * p_read_cplx->imagine;
		imagine = p_write_cplx->real * p_read_cplx->imagine + p_write_cplx->imagine * p_read_cplx->real;
		p_write_cplx->real = real;
		p_write_cplx->imagine = imagine;
		//加法
		p_read_cplx = addr(p_read_from[sample_start + sample_step * 2 * i]);
		p_write_cplx->real += p_read_cplx->real;
		p_write_cplx->imagine += p_read_cplx->imagine;
		//除二
		p_write_cplx->real /= 2.0f;
		p_write_cplx->imagine /= 2.0f;
	}
}

Suancai::Util::FFT::~FFT() {

	sdela(this->p_result0);
	sdela(this->p_result1_final);
}
