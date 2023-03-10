#pragma once

#include"__Suancai_Common.h"

#include<Windows.h>
#include<dxgi.h>
#include<d3d11.h>
#include<d2d1.h>
#include<imgui.h>
#include<imgui_impl_dx11.h>
#include<imgui_impl_win32.h>
#include"Common_Exception.h"
#include"FFT.h"
#include"Simple_Synchronization.h"
#include"Audio_Enumerator.h"
#include"Audio_Capturer.h"
#include<thread>

#pragma comment(lib, "d2d1")
#pragma comment(lib, "dxgi")
#pragma comment(lib, "d3d11")

namespace Suancai {

	namespace Vifft {

		using namespace Suancai::Util;
		using namespace Suancai::Media::Audio;

		unsigned __stdcall __Vifft_content_window_message_loop(void* p_arg);
		unsigned __stdcall __Audio_capture(void* p_arg);
		unsigned __stdcall __Vifft_content_window_message_loop_wrapper(void* p_arg);
		unsigned __stdcall __Audio_capture_wrapper(void* p_arg);

		class Vifft_config_ctx {
		public:
			class Vifft_d3d_drawing_context {
			public:
				HWND hwnd = NULL;
				ID3D11Device* g_pd3dDevice = NULL;
				ID3D11DeviceContext* g_pd3dDeviceContext = NULL;
				IDXGISwapChain* g_pSwapChain = NULL;
				ID3D11RenderTargetView* g_mainRenderTargetView = NULL;
				const char16_t* p_d3d_driver_used = nullptr;
			public:
				BOOL init(HWND hwnd);
				void create_render_target();
				void cleanup_render_target();
				void cleanup_all();
				~Vifft_d3d_drawing_context();
			};
			class Vifft_Config {
			public:
				class Color4 {
				public:
					float r = 1, g = 1, b = 1, a = 0.8f;
				};
			public:
				// * 数据与渲染速度
				//音频数据和渲染更新的速度
				enum class Power_Mode {
					Low = 0, High = 1
				};
				class Draw_Shape_Config {
				public:
					enum class Draw_Shape_Style {
						Line = 0, Rounded_Rect = 1
					};
					Draw_Shape_Style draw_shape_style = Draw_Shape_Style::Line;
					boolean is_shape_symmetric = true;
					float line_width_multiplier = 4.0f;
				};
				volatile Power_Mode power_mode = Power_Mode::Low;
				volatile u32 update_rate_ms = 33;//默认30fps
				// * fft相关
				volatile u32 fft_size = 1024;
				// * 样式相关
				volatile Draw_Shape_Config draw_shape_config;
				volatile u32 window_height = 256;
				volatile float amp_multiplier = 4.0f;
				//u32 draw_start_freq = 0;
				volatile u32 draw_freq_cnt = 256;
				volatile bool show_window_frame = false;
				volatile Color4 solid_color;
			};
		public:
			Vifft_Config cfg;
		public:
			HWND hwnd = NULL;
			Vifft_d3d_drawing_context d3d_ctx;
			//Simple_Mutex config_updated_lock;
			//Simple_Mutex config_lock;
		public:
			Vifft_config_ctx();
			~Vifft_config_ctx();
		};

		class Vifft_content_ctx {
		public:
			class Vifft_audio_capture_ctx {
			public:
				Simple_Mutex thread_ready;
				Audio_Enumerator enu;
				Audio_Capturer cap;
				FFT fft;
				Simple_Mutex fft_lock;

				// 如果为0表示没有音频数据，应该等待event
				// 太长时间电脑不播放音频的话，会设置为audio not ready，此时渲染线程会等待事件句柄
				u32 audio_ready_state = MAX_U32;
				HANDLE audio_ready_event = NULL;

				Vifft_audio_capture_ctx();
				~Vifft_audio_capture_ctx();
			};
			class Vifft_d2d_drawing_context {
			public:
				HWND hwnd = NULL;
				POINT wnd_pos = {};
				SIZE wnd_size = {};
				POINT point_00 = {};
				BLENDFUNCTION blend_config = {};
				HDC dc = NULL;
				HDC mem_dc = NULL;
				HBITMAP mem_bitmap = NULL;

				ID2D1Factory* p_d2d_fac = NULL;
				ID2D1DCRenderTarget* p_dcrt = NULL;
				ID2D1SolidColorBrush* p_solid_color_brush = NULL;
				//ID2D1PathGeometry* p_path = NULL;
				//ID2D1GeometrySink* p_path_sink = NULL;
			public:
				Vifft_d2d_drawing_context();
				BOOL init_d2d_backend(HWND hwnd);
				void resize_mem_dc_And_Render_target();
				~Vifft_d2d_drawing_context();
			};
		protected:
			//u32 cfg_updated = 0;
		public:
			Simple_Mutex thread_ready;
			HWND hwnd = NULL;
			Vifft_audio_capture_ctx aud_cap_ctx;
			Vifft_d2d_drawing_context d2d_ctx;
			//Simple_Mutex config_updated_lock;
			// * status
			volatile float cur_frame_time = 0;
			volatile float cur_frame_sleep_time = 0;
			volatile float update_layered_time = 0;
			volatile float fft_calc_time = 0;
			volatile float fft_sleep_time = 0;
			volatile u32 samples_cap = 0;
			volatile u32 audio_thread_aquire_fft_buffer_failed = 0;
			volatile u32 render_thread_aquire_fft_buffer_failed = 0;
			// * info
			u8string device_name;
		public:
			Vifft_content_ctx();
			~Vifft_content_ctx();
		};

		class Vifft_ctx_pack {
		public:
			Vifft_config_ctx config_ctx;
			Vifft_content_ctx content_ctx;
			HINSTANCE hInstance = NULL;
		};
	}
}