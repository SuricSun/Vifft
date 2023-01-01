#include "Vifft_Affair.h"


unsigned __stdcall Suancai::Vifft::__Vifft_content_window_message_loop_wrapper(void* p_arg) {
	
	Vifft_ctx_pack* p_pack = (Vifft_ctx_pack*)p_arg;
	Vifft_content_ctx* p_ctnt_ctx = addr(p_pack->content_ctx);
	Vifft_content_ctx::Vifft_audio_capture_ctx* p_aud_ctx = addr(p_ctnt_ctx->aud_cap_ctx);
	Vifft_content_ctx::Vifft_d2d_drawing_context* p_d2d_ctx = addr(p_ctnt_ctx->d2d_ctx);
	Vifft_config_ctx* p_cfg_ctx = addr(p_pack->config_ctx);
	Vifft_config_ctx::Vifft_Config* p_app_cfg = addr(p_cfg_ctx->cfg);

	LARGE_INTEGER cps = {};
	LARGE_INTEGER start_cnt = {};
	LARGE_INTEGER end_cnt = {};

	QueryPerformanceFrequency(addr(cps));

	//开FFT线程
	if (_beginthreadex(nullptr, 0, Suancai::Vifft::__Audio_capture, p_arg, 0, nullptr) == 0) {
		SUANCAI_THROW("无法启动音频捕获线程", -1, Suancai::Common_Exception::Base_exception);
	}
	//等待线程运行
	while (true) {
		if (p_aud_ctx->thread_ready.is_locked()) {
			//fft线程准备成功
			//把自己设置为准备成功
			p_ctnt_ctx->thread_ready.try_get();
			break;
		}
		Sleep(33);
	}

	WNDCLASS wc = { };

	wc.lpfnWndProc = DefWindowProc;
	wc.hInstance = p_pack->hInstance;
	wc.lpszClassName = L"Vifft_content_class";
	wc.hCursor = LoadCursorW(NULL, IDC_ARROW);

	RegisterClassW(&wc);

	int screenX = GetSystemMetrics(SM_CXSCREEN);
	int screenY = GetSystemMetrics(SM_CYSCREEN);

	u32 wnd_height = p_app_cfg->window_height;

	//Create the window.
	p_ctnt_ctx->hwnd = CreateWindowExW(
		WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOPMOST | WS_EX_NOACTIVATE, //NOACTIVE不会显示任务栏图标
		L"Vifft_content_class",
		L"Vifft Content",
		WS_POPUP | WS_DISABLED,
		0, screenY - wnd_height, screenX, wnd_height,
		NULL,
		NULL,
		p_pack->hInstance,
		NULL
	);

	if (p_ctnt_ctx->hwnd == NULL) {
		SUANCAI_THROW("无法创建vifft_content_wnd", -1, Suancai::Common_Exception::Base_exception);
	}

	ShowWindow(p_ctnt_ctx->hwnd, SW_SHOW);

	//初始化D2D
	if (p_d2d_ctx->init_d2d_backend(p_ctnt_ctx->hwnd) == FALSE) {
		SUANCAI_THROW("初始化D2D", -1, Suancai::Common_Exception::Base_exception);
	}

	p_d2d_ctx->p_dcrt->SetTransform(
		D2D1::Matrix3x2F::Scale(D2D1::SizeF(screenX, -256))
		*
		D2D1::Matrix3x2F::Translation(D2D1::SizeF(0, 256))
	);

	D2D1_ROUNDED_RECT rr;
	rr.radiusX = 100;
	rr.radiusY = 100;

	BOOL shouldContinue = TRUE;
	MSG msg;
	u32 cur_fft_size = p_app_cfg->fft_size;
	FFT::Complex* p_fft_series = new FFT::Complex[cur_fft_size];
	FFT::Complex* p_fft_series_new;
	u32 drawing_cnt = 0;
	float drawing_cnt_f = 0.0f;
	D2D1::ColorF color4 = D2D1::ColorF(
		p_app_cfg->solid_color.r, 
		p_app_cfg->solid_color.g, 
		p_app_cfg->solid_color.b, 
		1.0f
	);
	float h = 0;
	float r = 0, g = 0, b = 0;
	while (true) {
		// Poll and handle messages (inputs, window resize, etc.)
		// See the WndProc() function below for our to dispatch events to the Win32 backend.
		while (PeekMessageW(&msg, NULL, 0U, 0U, PM_REMOVE) != 0) {
			if (msg.message == WM_QUIT) {
				shouldContinue = false;
				break;
			}
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
		if (shouldContinue == false) {
			break;
		}
		if (IsWindowVisible(p_ctnt_ctx->hwnd)) {
			QueryPerformanceCounter(addr(start_cnt));
			//检查是否要更新窗口大小
			if (p_app_cfg->window_height != wnd_height) {
				wnd_height = p_app_cfg->window_height;
				screenX = GetSystemMetrics(SM_CXSCREEN);
				screenY = GetSystemMetrics(SM_CYSCREEN);
				//一定不能不能activate窗口
				SetWindowPos(p_ctnt_ctx->hwnd, NULL, 0, screenY - wnd_height, screenX, wnd_height, SWP_NOACTIVATE);
				p_d2d_ctx->resize_mem_dc_And_Render_target();
			}
			//一直获取fft（如果获取不到fft，渲染之前的fft的话，和不渲染是一样的，不如一直等待）
			while (p_aud_ctx->fft_lock.try_get_With_locked_data((void**)addr(p_fft_series_new)) == false);
			//成功获取
			//检查fft_size是否已经更新
			if (cur_fft_size != p_aud_ctx->fft.fft_size) {
				cur_fft_size = p_aud_ctx->fft.fft_size;
				sdela(p_fft_series);
				p_fft_series = new FFT::Complex[cur_fft_size];
				ZeroMemory(p_fft_series, sizeof(FFT::Complex) * cur_fft_size);
			}
			CopyMemory(p_fft_series, p_fft_series_new, sizeof(FFT::Complex) * cur_fft_size);
			p_aud_ctx->fft_lock.release();
			//开始渲染
			p_d2d_ctx->p_dcrt->BeginDraw();
			p_d2d_ctx->p_dcrt->Clear(D2D1::ColorF(0, 0, 0, 0));
			//设置画刷透明度
			p_d2d_ctx->p_solid_color_brush->SetOpacity(p_app_cfg->solid_color.a);
			drawing_cnt = p_app_cfg->draw_freq_cnt;
			//防止越界
			if (drawing_cnt > cur_fft_size) {
				drawing_cnt = cur_fft_size;
			}
			drawing_cnt_f = (float)drawing_cnt;
			for (int i = 0; i < drawing_cnt; i++) {
				ImGui::ColorConvertHSVtoRGB(i / drawing_cnt_f + p_fft_series[i].real, 1.0f, 1.0f, r, g, b);
				color4.r = /*p_app_cfg->solid_color.*/r;
				color4.g = /*p_app_cfg->solid_color.*/g;
				color4.b = /*p_app_cfg->solid_color.*/b;
				p_d2d_ctx->p_solid_color_brush->SetColor(color4);
				rr.rect = D2D1::RectF(
					i / drawing_cnt_f,
					0,
					i / drawing_cnt_f + 1.0f / drawing_cnt_f,
					p_fft_series[i].real * p_app_cfg->amp_factor
				);
				p_d2d_ctx->p_dcrt->FillRoundedRectangle(
					rr,
					p_d2d_ctx->p_solid_color_brush
				);
			}
			p_d2d_ctx->p_solid_color_brush->SetOpacity(0.382);
			if (p_app_cfg->show_window_frame) {
				p_d2d_ctx->p_dcrt->FillRectangle(
					D2D1::RectF(0, 0, 1, 1),
					p_d2d_ctx->p_solid_color_brush
				);
			}
			p_d2d_ctx->p_dcrt->EndDraw();
			QueryPerformanceCounter(addr(end_cnt));
			p_ctnt_ctx->cur_frame_time = ((float)end_cnt.QuadPart - (float)start_cnt.QuadPart) / (float)cps.QuadPart * 1000.0f;
			//update transparent window
			QueryPerformanceCounter(addr(start_cnt));
			RECT r;
			GetWindowRect(p_ctnt_ctx->hwnd, addr(r));
			p_d2d_ctx->wnd_pos.x = r.left;
			p_d2d_ctx->wnd_pos.y = r.top;
			p_d2d_ctx->wnd_size.cx = r.right - r.left;
			p_d2d_ctx->wnd_size.cy = r.bottom - r.top;
			if (UpdateLayeredWindow(
				p_d2d_ctx->hwnd,
				p_d2d_ctx->dc,
				&p_d2d_ctx->wnd_pos,
				&p_d2d_ctx->wnd_size,
				p_d2d_ctx->mem_dc,
				&p_d2d_ctx->point_00,
				0,
				&p_d2d_ctx->blend_config,
				ULW_ALPHA) == FALSE) {

				SUANCAI_THROW("无法设置窗口透明属性(UpdateLayeredWindow)", -1, Suancai::Common_Exception::Base_exception);
				DestroyWindow(p_ctnt_ctx->hwnd);
				continue;
			}
			if (IsWindowVisible(p_ctnt_ctx->hwnd)) {
				BringWindowToTop(p_ctnt_ctx->hwnd);
			}
			QueryPerformanceCounter(addr(end_cnt));
			p_ctnt_ctx->update_layered_time = ((float)end_cnt.QuadPart - (float)start_cnt.QuadPart) / (float)cps.QuadPart * 1000.0f;
		}
		QueryPerformanceCounter(addr(start_cnt));
		Sleep(p_app_cfg->update_rate_ms);
		QueryPerformanceCounter(addr(end_cnt));
		p_ctnt_ctx->cur_frame_sleep_time = ((float)end_cnt.QuadPart - (float)start_cnt.QuadPart) / (float)cps.QuadPart * 1000.0f;
	}

	return 0;
}

unsigned __stdcall Suancai::Vifft::__Audio_capture_wrapper(void* p_arg) {
	
	Vifft_ctx_pack* p_pack = (Vifft_ctx_pack*)p_arg;
	Vifft_content_ctx* p_ctnt_ctx = addr(p_pack->content_ctx);
	Vifft_content_ctx::Vifft_audio_capture_ctx* p_aud_ctx = addr(p_ctnt_ctx->aud_cap_ctx);
	Vifft_content_ctx::Vifft_d2d_drawing_context* p_d2d_ctx = addr(p_ctnt_ctx->d2d_ctx);
	Vifft_config_ctx* p_cfg_ctx = addr(p_pack->config_ctx);
	Vifft_config_ctx::Vifft_Config* p_app_cfg = addr(p_cfg_ctx->cfg);

	LARGE_INTEGER cps = {};
	LARGE_INTEGER start_cnt = {};
	LARGE_INTEGER end_cnt = {};

	QueryPerformanceFrequency(addr(cps));

	//Settings
	u32 capture_fft_size = 0;

	//开始运行捕获音频并且计算FFT
	FFT* p_fft = addr(p_aud_ctx->fft);
	u32 cur_fft_size = p_app_cfg->fft_size;
	p_fft->init_buffer(cur_fft_size);
	p_aud_ctx->fft_lock = Simple_Mutex(p_fft->p_result1_final);

	Audio_Enumerator* p_enu = addr(p_aud_ctx->enu);
	Audio_Capturer* p_cap = addr(p_aud_ctx->cap);
	p_enu->init();
	//TODO: 1秒间隔也需要改
	p_cap->init(p_enu->get_default_render_device(), 10000000, true);
	if (p_cap->audio_fmt != Audio_sample_Format::IEEE_Float || p_cap->sample_bit_depth != sizeof(float) * 8) {
		SUANCAI_THROW("目前仅支持[IEEE_Float 32位深 任意通道类型]类型的音频流", -1, Suancai::Media::Audio::Audio_Exception);
	}

	u32 channels = p_cap->channels;

	IAudioCaptureClient* p_cap_cli = p_cap->get_capture_client();

	u32 nums_available = 0;
	float* p_data = nullptr;

	float* p_fft_sample_buffer = new float[cur_fft_size];
	u32 buffer_start = 0;
	ZeroMemory(p_fft_sample_buffer, sizeof(float) * cur_fft_size);

	HRESULT hr = S_OK;
	DWORD flag_returned;

	UTFStringAffair::UTF16To8(p_cap->device_name, p_ctnt_ctx->device_name);

	//音频捕获线程准备完成
	p_aud_ctx->thread_ready.try_get();

	p_cap->enable_capture();

	u32 samples_cap = 0;
	bool has_samples = false;
	while (true) {
		QueryPerformanceCounter(addr(start_cnt));
		Sleep(p_app_cfg->update_rate_ms);
		QueryPerformanceCounter(addr(end_cnt));
		p_ctnt_ctx->fft_sleep_time = ((float)end_cnt.QuadPart - (float)start_cnt.QuadPart) / (float)cps.QuadPart * 1000.0f;
		
		samples_cap = 0;
		p_cap_cli->GetNextPacketSize(addr(nums_available));
		while (nums_available > 0) {
			has_samples = true;
			hr = p_cap_cli->GetBuffer((BYTE**)addr(p_data), addr(nums_available), addr(flag_returned), NULL, NULL);
			if (FAILED(hr)) {
				//TODO: throw exception
				exit(-100);
			}
			//复制数据，只获取p_data的最后fft_size部分的内容因为前面部分会被覆盖掉
			//TODO: 可能有隐患（我不知道），出问题关此处功能
			samples_cap += nums_available;
			if (nums_available > cur_fft_size) {
				p_data += channels * (nums_available - cur_fft_size);
				nums_available = cur_fft_size;
			}
			for (u32 i = 0; i < nums_available; i++) {
				p_fft_sample_buffer[buffer_start] = p_data[channels * i];
				//p_fft_sample_buffer[buffer_start + 1] = p_data[2 * i + 1];
				buffer_start += 1;
				buffer_start %= cur_fft_size;
			}
			p_cap_cli->ReleaseBuffer(nums_available);
			p_cap_cli->GetNextPacketSize(addr(nums_available));
		}
		if (has_samples) {
			//做傅里叶拿不到就不计算
			if (p_aud_ctx->fft_lock.try_get()) {
				//（拿到fft锁才能init）是否要重新init buffer
				if (cur_fft_size != p_app_cfg->fft_size) {
					cur_fft_size = p_app_cfg->fft_size;
					sdela(p_fft_sample_buffer);
					p_fft_sample_buffer = new float[cur_fft_size];
					ZeroMemory(p_fft_sample_buffer, sizeof(float) * cur_fft_size);
					buffer_start = 0;
					p_fft->init_buffer(cur_fft_size);
				}
				QueryPerformanceCounter(addr(start_cnt));
				p_fft->do_fft(p_fft_sample_buffer, buffer_start, cur_fft_size, 0, 1, cur_fft_size);
				QueryPerformanceCounter(addr(end_cnt));
				p_ctnt_ctx->fft_calc_time = ((float)end_cnt.QuadPart - (float)start_cnt.QuadPart) / (float)cps.QuadPart * 1000.0f;
				p_aud_ctx->fft_lock.release_With_locked_data(p_fft->p_result1_final);
			} else {
				p_ctnt_ctx->audio_thread_aquire_fft_buffer_failed++;
			}
		}
		p_ctnt_ctx->samples_cap = samples_cap;
	}

	p_cap->disable_capture();

	return 0;
}

unsigned __stdcall Suancai::Vifft::__Vifft_content_window_message_loop(void* p_arg) {

	try {
		__Vifft_content_window_message_loop_wrapper(p_arg);
	} catch(Suancai::Common_Exception::Base_exception* p_e) {
		p_e->showMsg();
		sdel(p_e);
	}
	return 0;
}

unsigned __stdcall Suancai::Vifft::__Audio_capture(void* p_arg) {

	try {
		__Audio_capture_wrapper(p_arg);
	} catch (Suancai::Common_Exception::Base_exception* p_e) {
		p_e->showMsg();
		sdel(p_e);
	}
	return 0;
}

Suancai::Vifft::Vifft_content_ctx::Vifft_d2d_drawing_context::Vifft_d2d_drawing_context() {
}

BOOL Suancai::Vifft::Vifft_content_ctx::Vifft_d2d_drawing_context::init_d2d_backend(HWND hwnd) {

	this->hwnd = hwnd;

	// Obtain the size of the drawing area.
	RECT client_rect = {};
	GetClientRect(hwnd, addr(client_rect));

	this->point_00 = {0, 0};

	blend_config.BlendOp = AC_SRC_OVER;
	blend_config.BlendFlags = 0;
	blend_config.SourceConstantAlpha = 0xFFU;
	blend_config.AlphaFormat = AC_SRC_ALPHA;
	this->dc = GetDC(NULL);
	this->mem_dc = CreateCompatibleDC(this->dc);
	this->mem_bitmap = CreateCompatibleBitmap(this->dc, client_rect.right, client_rect.bottom);
	SelectObject(this->mem_dc, this->mem_bitmap);

	HRESULT hr = S_OK;

	hr = D2D1CreateFactory(
		D2D1_FACTORY_TYPE::D2D1_FACTORY_TYPE_SINGLE_THREADED,
		addr(this->p_d2d_fac)
	);

	if (SUCCEEDED(hr) == FALSE) {
		SUANCAI_THROW("初始化错误: D2D1CreateFactory", -1, Suancai::Common_Exception::Base_exception);
	}

	// Create a Direct2D render target          
	D2D1_RENDER_TARGET_PROPERTIES dcrt_props = D2D1::RenderTargetProperties(
		D2D1_RENDER_TARGET_TYPE::D2D1_RENDER_TARGET_TYPE_DEFAULT,
		D2D1::PixelFormat(
			DXGI_FORMAT::DXGI_FORMAT_B8G8R8A8_UNORM,
			D2D1_ALPHA_MODE::D2D1_ALPHA_MODE_PREMULTIPLIED),
		96.0f,
		96.0f,
		D2D1_RENDER_TARGET_USAGE::D2D1_RENDER_TARGET_USAGE_NONE,
		D2D1_FEATURE_LEVEL::D2D1_FEATURE_LEVEL_DEFAULT
	);

	hr = this->p_d2d_fac->CreateDCRenderTarget(
		&dcrt_props,
		addr(this->p_dcrt)
	);

	if (SUCCEEDED(hr) == FALSE) {
		SUANCAI_THROW("初始化错误: CreateHwndRenderTarget", -1, Suancai::Common_Exception::Base_exception);
	}

	if (FAILED(this->p_dcrt->BindDC(mem_dc, &client_rect))) {
		SUANCAI_THROW("初始化错误: this->p_dcrt->BindDC(mem_dc, &client_rect)", -1, Suancai::Common_Exception::Base_exception);
	}
	this->p_dcrt->SetDpi(96.0, 96.0f);
	this->p_dcrt->SetAntialiasMode(D2D1_ANTIALIAS_MODE::D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
	this->p_dcrt->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE::D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);

	hr = this->p_dcrt->CreateSolidColorBrush(
		D2D1::ColorF(D2D1::ColorF::White),
		addr(this->p_solid_color_brush)
	);

	if (SUCCEEDED(hr) == FALSE) {
		SUANCAI_THROW("初始化错误: CreateSolidColorBrush", -1, Suancai::Common_Exception::Base_exception);
	}

	return TRUE;

	//D2D1_GRADIENT_STOP gradient_stops[5];
	//gradient_stops[0].color = D2D1::ColorF(D2D1::ColorF::Red, 0);
	//gradient_stops[0].position = 0.0f;
	//gradient_stops[1].color = D2D1::ColorF(D2D1::ColorF::Red, 0.75);
	//gradient_stops[1].position = 0.25f;
	//gradient_stops[2].color = D2D1::ColorF(D2D1::ColorF::Red, 1);
	//gradient_stops[2].position = 0.5f;
	//gradient_stops[3].color = D2D1::ColorF(D2D1::ColorF::Red, 0.75);
	//gradient_stops[3].position = 0.75f;
	//gradient_stops[4].color = D2D1::ColorF(D2D1::ColorF::Red, 0);
	//gradient_stops[4].position = 1.0f;

	//hr = this->p_dcrt->CreateGradientStopCollection(
	//	gradient_stops,
	//	ARRAYSIZE(gradient_stops),
	//	D2D1_GAMMA_2_2,
	//	D2D1_EXTEND_MODE_CLAMP,
	//	addr(this->p_gradient_stop_collection)
	//);

	//if (SUCCEEDED(hr) == FALSE) {
	//	Say_and_ret_neg1(L"CreateGradientStopCollection");
	//}

	//hr = p_dcrt->CreateLinearGradientBrush(
	//	D2D1::LinearGradientBrushProperties(D2D1::Point2F(0, 0), D2D1::Point2F(wnd_rect.right, 0)),
	//	D2D1::BrushProperties(),
	//	this->p_gradient_stop_collection,
	//	addr(this->p_linear_gradient_brush)
	//);

	//if (SUCCEEDED(hr) == FALSE) {
	//	Say_and_ret_neg1(L"CreateLinearGradientBrush");
	//}
	//return TRUE;
}

void Suancai::Vifft::Vifft_content_ctx::Vifft_d2d_drawing_context::resize_mem_dc_And_Render_target() {

	//ReleaseDC(NULL, this->dc);
	DeleteDC(mem_dc);

	// Obtain the size of the drawing area.
	RECT client_rect = {};
	GetClientRect(hwnd, addr(client_rect));

	//this->dc = GetDC(NULL);
	this->mem_dc = CreateCompatibleDC(this->dc);
	this->mem_bitmap = CreateCompatibleBitmap(this->dc, client_rect.right, client_rect.bottom);
	SelectObject(this->mem_dc, this->mem_bitmap);

	this->p_dcrt->BindDC(this->mem_dc, addr(client_rect));

	this->p_dcrt->SetTransform(
		D2D1::Matrix3x2F::Scale(D2D1::SizeF((float)client_rect.right, -1.0f * (float)client_rect.bottom))
		*
		D2D1::Matrix3x2F::Translation(D2D1::SizeF(0, client_rect.bottom))
	);
}

Suancai::Vifft::Vifft_content_ctx::Vifft_d2d_drawing_context::~Vifft_d2d_drawing_context() {

	Safe_Release(this->p_solid_color_brush);
	//Safe_Release(this->p_gradient_stop_collection);
	//Safe_Release(this->p_linear_gradient_brush);
	Safe_Release(this->p_dcrt);
	Safe_Release(this->p_d2d_fac);

	//TODO: NULL means?
	ReleaseDC(NULL, dc);
	DeleteDC(mem_dc);
	DeleteObject(mem_bitmap);
}

BOOL Suancai::Vifft::Vifft_config_ctx::Vifft_d3d_drawing_context::init(HWND hwnd) {

	this->hwnd = hwnd;
	// Setup swap chain
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 2;
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT::DXGI_FORMAT_B8G8R8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 0;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hwnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT::DXGI_SWAP_EFFECT_SEQUENTIAL;

	UINT createDeviceFlags = 0;
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
	D3D_FEATURE_LEVEL featureLevelTaken;
	const D3D_FEATURE_LEVEL featureLevelArray[] = {
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1,
	};
	HRESULT hr = S_OK;
	hr = D3D11CreateDevice(NULL, D3D_DRIVER_TYPE::D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevelArray, ARRAYSIZE(featureLevelArray), D3D11_SDK_VERSION, &g_pd3dDevice, &featureLevelTaken, &g_pd3dDeviceContext);
	if (hr != S_OK) {
		hr = D3D11CreateDevice(NULL, D3D_DRIVER_TYPE::D3D_DRIVER_TYPE_WARP, NULL, createDeviceFlags, featureLevelArray, ARRAYSIZE(featureLevelArray), D3D11_SDK_VERSION, &g_pd3dDevice, &featureLevelTaken, &g_pd3dDeviceContext);
		if (hr != S_OK) {
			SUANCAI_THROW("无法创建D3D11设备", -1, Suancai::Common_Exception::Base_exception);
			return FALSE;
		} else {
			this->p_d3d_driver_used = u"Fucking Exam 软件驱动";
		}
	} else {
		this->p_d3d_driver_used = u"Fucking Exam 硬件驱动";
	}

	IDXGIDevice* p_dxgi_device = nullptr;
	IDXGIAdapter* p_dxgi_adapter = nullptr;
	IDXGIFactory* p_dxgi_factory = nullptr;
	hr = g_pd3dDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&p_dxgi_device);
	hr = p_dxgi_device->GetParent(__uuidof(IDXGIAdapter), (void**)&p_dxgi_adapter);
	hr = p_dxgi_adapter->GetParent(__uuidof(IDXGIFactory), (void**)&p_dxgi_factory);
	hr = p_dxgi_factory->CreateSwapChain(g_pd3dDevice, &sd, &g_pSwapChain);
	if (hr != S_OK) {
		SUANCAI_THROW("无法创建交换链", -1, Suancai::Common_Exception::Base_exception);
		return FALSE;
	}

	this->create_render_target();

	return TRUE;
}

void Suancai::Vifft::Vifft_config_ctx::Vifft_d3d_drawing_context::create_render_target() {

	ID3D11Texture2D* pBackBuffer;
	if (FAILED(g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer)))) {
		SUANCAI_THROW("无法获取交换链的序号为0的后背缓冲区", -1, Suancai::Common_Exception::Base_exception);
	}
	g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_mainRenderTargetView);
	pBackBuffer->Release();
}

void Suancai::Vifft::Vifft_config_ctx::Vifft_d3d_drawing_context::cleanup_render_target() {

	if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = NULL; }
}

void Suancai::Vifft::Vifft_config_ctx::Vifft_d3d_drawing_context::cleanup_all() {

	cleanup_render_target();
	if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = NULL; }
	if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = NULL; }
	if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
}

Suancai::Vifft::Vifft_config_ctx::Vifft_d3d_drawing_context::~Vifft_d3d_drawing_context() {

	//析构函数不应抛出异常
	//在析构函数中默默吞掉异常
	try {
		this->cleanup_all();
	} catch (Suancai::Common_Exception::Base_exception* p_e) {
		sdel(p_e);
	}
}

Suancai::Vifft::Vifft_config_ctx::Vifft_config_ctx() {

	//this->config_lock = Simple_Mutex(addr(this->cfg));
	//this->config_updated_lock = Simple_Mutex(addr(this->cfg_updated));
}

Suancai::Vifft::Vifft_config_ctx::~Vifft_config_ctx() {

	//do nothing
}

Suancai::Vifft::Vifft_content_ctx::Vifft_content_ctx() {

	//this->config_updated_lock = Simple_Mutex(addr(this->cfg_updated));
}

Suancai::Vifft::Vifft_content_ctx::~Vifft_content_ctx() {

	//do nothing
}
