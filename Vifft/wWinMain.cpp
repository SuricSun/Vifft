#include<Windows.h>
#include<dxgi.h>
#include<d3d11.h>
#include<imgui.h>
#include<imgui_impl_dx11.h>
#include<imgui_impl_win32.h>
#include"Vifft_Affair.h"
#include<thread>

#define U8(arg) u8##arg
#define u8char(arg) ((char*)u8##arg)

static HICON main_icon = NULL;

static HWND root_hided_wnd = NULL;
static HWND vifft_content_wnd = NULL;
static HWND vifft_config_wnd = NULL;

static HMENU h_notify_icon_menu = NULL;
static NOTIFYICONDATAW notify_icon_data;

void Fire(HINSTANCE hInstance);
IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI Vifft_config_Proc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL CreateSystemTrayAndMenu(HWND hwnd_that_the_tray_belongs);
const ImWchar* GetMyGlyphRange();

using namespace Suancai;
using namespace Suancai::Vifft;

static Vifft_ctx_pack Ctx_Pack;

INT WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ PWSTR lpCmdLine, _In_ INT nCmdShow) {

	try {
		Fire(hInstance);
	} catch (Suancai::Common_Exception::Base_exception* p_e) {
		p_e->showMsg();
		sdel(p_e);
	}
	return 0;
}

void Fire(HINSTANCE hInstance) {
	
	//TODO:单例

	/*
	* 创建窗口（隐藏到任务栏）
	*/
	#pragma region MyRegion

	Ctx_Pack.hInstance = hInstance;

	WNDCLASS wc = { };

	wc.lpfnWndProc = Vifft_config_Proc;
	wc.hInstance = hInstance;
	wc.lpszClassName = L"Vifft_config_class";
	wc.hCursor = LoadCursorW(NULL, IDC_ARROW);
	main_icon = LoadIcon(hInstance, L"$Aerith");
	if (main_icon == NULL) {
		//Set_and_show_global_err_msg_and_ret(L"初始化错误", L"无法加载Aerith图标", -1);
		exit(-1);
	}
	wc.hIcon = main_icon;

	RegisterClassW(&wc);

	int screenX = GetSystemMetrics(SM_CXSCREEN);
	int screenY = GetSystemMetrics(SM_CYSCREEN);

	// Create the window.
	vifft_config_wnd = CreateWindowExW(
		0,										// Optional window styles.
		L"Vifft_config_class",											// Window class
		L"Vifft Config",						// Window text
		WS_OVERLAPPEDWINDOW,        // Window style
		screenX / 4, screenY / 4, screenX / 2, screenY / 2, // Size and position
		NULL,										// Parent window    
		NULL,												// Menu
		hInstance,											// Instance handle
		NULL												// Additional application data
	);

	if (vifft_config_wnd == NULL) {
		SUANCAI_THROW("无法创建vifft_config_wnd", -1, Suancai::Common_Exception::Base_exception);
	}

	Ctx_Pack.config_ctx.hwnd = vifft_config_wnd;

	//创建托盘
	if (CreateSystemTrayAndMenu(vifft_config_wnd) == FALSE) {
		SUANCAI_THROW("无法创建托盘图标", -1, Suancai::Common_Exception::Base_exception);
	}

	#pragma endregion

	/*
	* 加载配置文件
	*/
	#pragma region MyRegion

	#pragma endregion

	/*
	* 进入消息循环
	*/
	#pragma region MyRegion

	//初始化D3D
	if (Ctx_Pack.config_ctx.d3d_ctx.init(vifft_config_wnd) == FALSE) {
		SUANCAI_THROW("无法初始化主窗口Direct3D上下文!", -1, Suancai::Common_Exception::Base_exception);
	}

	//初始化IMGUI
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsLight();
	//ImGui::StyleColorsClassic();

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(vifft_config_wnd);
	ImGui_ImplDX11_Init(Ctx_Pack.config_ctx.d3d_ctx.g_pd3dDevice, Ctx_Pack.config_ctx.d3d_ctx.g_pd3dDeviceContext);

	// Load Fonts
	// - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
	// - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
	// - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
	// - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
	// - Read 'docs/FONTS.md' for more instructions and details.
	// - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
	//io.Fonts->AddFontDefault();
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
	//ImFont* font = io.Fonts->AddFontFromFileTTF(U8("C:\\Windows\\Fonts\\SIMYOU.TTF"), 15.0f, NULL, io.Fonts->GetGlyphRangesChineseFull());
	ImFont* p_font = io.Fonts->AddFontFromFileTTF((char*)u8"C:\\Windows\\Fonts\\msyh.ttc", 18, NULL, io.Fonts->GetGlyphRangesChineseFull());
	//IM_ASSERT(font != NULL);
	// * 设置ImGui全局样式
	ImGuiStyle& ref_style = ImGui::GetStyle();
	ref_style.FrameBorderSize = 0;
	ref_style.WindowBorderSize = 0;
	//下面两个rounding保持统一
	ref_style.FrameRounding = 3.14f;
	ref_style.GrabRounding = 3.14f;

	ShowWindow(vifft_config_wnd, SW_SHOW);

	// 消息有两种，queued message（放入PostMessage消息队列）和nonqueued message（放入SendMessage消息队列）
	// PeekMessage解释:
	// During this call, the system dispatches(DispatchMessage) pending, nonqueued messages, 
	// that is, messages sent to windows owned by the calling thread using the SendMessage, SendMessageCallback, SendMessageTimeout, or SendNotifyMessage function.
	// Then the first queued message that matches the specified filter is retrieved.
	// The system may also process internal events.If no filter is specified, messages are processed in the following order :
	//
	//   Sent messages
	//	 Posted messages
	//	 Input(hardware) messages and system internal events
	//	 Sent messages(again)
	//	 WM_PAINT messages
	//	 WM_TIMER messages
	// 
	// 
	// 
	
	/* 
	* Main loop start
	*/

	//TODO: 可以增加选项设置进程优先级
	if (SetPriorityClass(GetCurrentProcess(), BELOW_NORMAL_PRIORITY_CLASS) == FALSE) {
		SUANCAI_THROW("无法设置进程调度优先级", -1, Suancai::Common_Exception::Base_exception);
	}

	//开渲染线程
	if (_beginthreadex(nullptr, 0, Suancai::Vifft::__Vifft_content_window_message_loop, addr(Ctx_Pack), 0, nullptr) == 0) {
		SUANCAI_THROW("无法创建渲染工作线程", -1, Suancai::Common_Exception::Base_exception);
	}

	//等待启动
	while (true) {
		if (Ctx_Pack.content_ctx.thread_ready.is_locked()) {
			break;
		}
		Sleep(33);
	}

	/*
	* IMGUI用户数据
	*/
	Vifft_config_ctx::Vifft_Config* p_cfg = addr(Ctx_Pack.config_ctx.cfg);

	float clear_color[4] = {0.0f, 0.0f, 0.0f, 0.0f};
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings;

	const char* update_interval_name[] = {u8char("30fps"), u8char("60fps")};
	const u32 update_interval_value[] = {33, 17, 0};
	//默认0，也就是30fps
	int update_interval_selected_idx = 0;

	const char* fft_size_name[] = {u8char("1024"), u8char("2048")};
	const u32 fft_size_value[] = {1024, 2048};
	//默认0，也就是1024samples
	int fft_size_selected_idx = 0;

	const char* power_mode_name[] = {u8char("极致省电"), u8char("极致费电")};
	const Vifft_config_ctx::Vifft_Config::Power_Mode power_mode_value[] = {
		Vifft_config_ctx::Vifft_Config::Power_Mode::Low, 
		Vifft_config_ctx::Vifft_Config::Power_Mode::High
	};
	//默认0，也就是1024samples
	int power_mode_selected_idx = 0;

	const char* draw_shape_name[] = {u8char("线条"), u8char("圆角矩形")};
	const Vifft_config_ctx::Vifft_Config::Draw_Shape_Config::Draw_Shape_Style draw_shape_value[] = {
		Vifft_config_ctx::Vifft_Config::Draw_Shape_Config::Draw_Shape_Style::Line,
		Vifft_config_ctx::Vifft_Config::Draw_Shape_Config::Draw_Shape_Style::Rounded_Rect
	};
	//默认0，也就是1024samples
	int draw_shape_selected_idx = 0;

	i32 window_height = p_cfg->window_height;
	i32 uniform_temp_i32 = 0;
	float uniform_temp_float = 0.0f;

	/*
	* 开始消息循环
	*/
	bool shouldContinue = true;
	MSG msg;
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
		//渲染Vifft config窗口
		if (IsWindowVisible(vifft_config_wnd)) {
			// Start the Dear ImGui frame
			ImGui_ImplDX11_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();

			// We demonstrate using the full viewport area or the work area (without menu-bars, task-bars etc.)
			// Based on your use case you may want one of the other.
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->Pos);
			ImGui::SetNextWindowSize(viewport->Size);

			if (ImGui::Begin(u8char("##root"), nullptr, window_flags)) {
				if (ImGui::BeginTabBar(u8char("##root_tab"), ImGuiTabBarFlags_::ImGuiTabBarFlags_None)) {
					if (ImGui::BeginTabItem(u8char("工作线程设置"), nullptr, ImGuiTabItemFlags_::ImGuiTabItemFlags_None)) {
						if (ImGui::BeginChild(u8char("##root_tab_child"))) {
							if (ImGui::TreeNode(u8char("性能"))) {
								ImGui::AlignTextToFramePadding();
								ImGui::Text(u8char("性能选项"));
								ImGui::SameLine();
								ImGui::PushItemWidth(128);
								if (ImGui::BeginCombo(u8char("##性能选项"), power_mode_name[power_mode_selected_idx])) {
									for (int n = 0; n < IM_ARRAYSIZE(power_mode_name); n++) {
										const bool is_selected = (power_mode_selected_idx == n);
										//如果ImGui::Selectable(...)返回真代表本帧被点击
										if (ImGui::Selectable(power_mode_name[n], is_selected)) {
											power_mode_selected_idx = n;
											//更新设置
											p_cfg->power_mode = power_mode_value[power_mode_selected_idx];
										}
										// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
										if (is_selected) {
											ImGui::SetItemDefaultFocus();
										}
									}
									ImGui::EndCombo();
								}
								ImGui::PopItemWidth();

								ImGui::AlignTextToFramePadding();
								ImGui::Text(u8char("帧率"));
								ImGui::SameLine();
								ImGui::PushItemWidth(128);
								if (ImGui::BeginCombo(u8char("##帧率"), update_interval_name[update_interval_selected_idx])) {
									for (int n = 0; n < IM_ARRAYSIZE(update_interval_name); n++) {
										const bool is_selected = (update_interval_selected_idx == n);
										//如果ImGui::Selectable(...)返回真代表本帧被点击
										if (ImGui::Selectable(update_interval_name[n], is_selected)) {
											update_interval_selected_idx = n;
											//更新设置
											p_cfg->update_rate_ms = update_interval_value[update_interval_selected_idx];
										}
										// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
										if (is_selected) {
											ImGui::SetItemDefaultFocus();
										}
									}
									ImGui::EndCombo();
								}
								ImGui::PopItemWidth();

								ImGui::AlignTextToFramePadding();
								ImGui::Text(u8char("傅里叶级数数量"));
								ImGui::SameLine();
								ImGui::PushItemWidth(128);
								if (ImGui::BeginCombo(u8char("##傅里叶级数数量"), fft_size_name[fft_size_selected_idx])) {
									for (int n = 0; n < IM_ARRAYSIZE(fft_size_name); n++) {
										const bool is_selected = (fft_size_selected_idx == n);
										//如果ImGui::Selectable(...)返回真代表本帧被点击
										if (ImGui::Selectable(fft_size_name[n], is_selected)) {
											fft_size_selected_idx = n;
											//更新设置
											p_cfg->fft_size = fft_size_value[fft_size_selected_idx];
										}
										// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
										if (is_selected) {
											ImGui::SetItemDefaultFocus();
										}
									}
									ImGui::EndCombo();
								}
								ImGui::PopItemWidth();

								ImGui::TreePop();
							}
							if (ImGui::TreeNode(u8char("样式"))) {
								if (ImGui::TreeNode(u8char("形状设置"))) {
									ImGui::AlignTextToFramePadding();
									ImGui::Text(u8char("形状"));
									ImGui::SameLine();
									ImGui::PushItemWidth(128);
									if (ImGui::BeginCombo(u8char("##形状"), draw_shape_name[draw_shape_selected_idx])) {
										for (int n = 0; n < IM_ARRAYSIZE(draw_shape_name); n++) {
											const bool is_selected = (draw_shape_selected_idx == n);
											//如果ImGui::Selectable(...)返回真代表本帧被点击
											if (ImGui::Selectable(draw_shape_name[n], is_selected)) {
												draw_shape_selected_idx = n;
												//更新设置
												p_cfg->draw_shape_config.draw_shape_style = draw_shape_value[draw_shape_selected_idx];
											}
											// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
											if (is_selected) {
												ImGui::SetItemDefaultFocus();
											}
										}
										ImGui::EndCombo();
									}
									ImGui::PopItemWidth();

									ImGui::AlignTextToFramePadding();
									ImGui::Text(u8char("线条粗细"));
									ImGui::SameLine();
									uniform_temp_float = Ctx_Pack.config_ctx.cfg.draw_shape_config.line_width_multiplier;
									ImGui::SliderFloat(u8char("##"), addr(uniform_temp_float), 0.0f, 64.0f);
									Ctx_Pack.config_ctx.cfg.draw_shape_config.line_width_multiplier = uniform_temp_float;

									ImGui::TreePop();
								}

								ImGui::AlignTextToFramePadding();
								ImGui::Text(u8char("渲染数量"));
								ImGui::SameLine();
								uniform_temp_i32 = p_cfg->draw_freq_cnt;
								ImGui::SliderInt("##渲染数量", addr(uniform_temp_i32), 1, p_cfg->fft_size);
								p_cfg->draw_freq_cnt = abs(uniform_temp_i32); // 有点傻逼的写法，直接assert完了呗

								p_cfg->show_window_frame = false;

								ImGui::AlignTextToFramePadding();
								ImGui::Text(u8char("频谱高度"));
								ImGui::SameLine();
								uniform_temp_float = p_cfg->amp_multiplier;
								ImGui::SliderFloat("##频谱高度", addr(uniform_temp_float), 0.0, 64.0);
								p_cfg->amp_multiplier = uniform_temp_float;
								if (ImGui::IsItemActive()) {
									p_cfg->show_window_frame = true;
								}

								ImGui::AlignTextToFramePadding();
								ImGui::Text(u8char("窗口高度"));
								ImGui::SameLine();
								ImGui::SliderInt("##窗口高度", addr(window_height), 1, screenY);
								if (ImGui::IsItemActive()) {
									p_cfg->show_window_frame = true;
								}
								if (ImGui::IsItemDeactivated()) {
									//只有松开的时候才更新因为更新窗口大小做的工作比较费时间
									p_cfg->window_height = window_height;
								}

								//ImGui::AlignTextToFramePadding();
								//ImGui::Text(u8char("线条粗细"));
								//ImGui::SameLine();
								//ImGui::ColorEdit4("MyColor##3", (float*)&color4,
								//				  ImGuiColorEditFlags_NoInputs |
								//				  ImGuiColorEditFlags_NoLabel |
								//				  ImGuiColorEditFlags_Float |
								//				  ImGuiColorEditFlags_AlphaPreview |
								//				  ImGuiColorEditFlags_AlphaBar
								//);
								//p_cfg->solid_color.r = color4.x;
								//p_cfg->solid_color.g = color4.y;
								//p_cfg->solid_color.b = color4.z;
								//p_cfg->solid_color.a = color4.w;

								ImGui::TreePop();
							}
							/*
							* 工作线程状态
							*/
							ImGui::Separator();
							ImGui::Text(u8char("工作线程状态: "));
							ImGui::SameLine();
							if (Ctx_Pack.content_ctx.thread_ready.is_locked()) {
								ImGui::TextColored(ImVec4(0, 1, 0, 1), u8char("运行中"));
							} else {
								ImGui::TextColored(ImVec4(0.5, 0.5, 0.5, 1), u8char("停止"));
							}
							ImGui::Text(u8char("声卡名称: %s"), Ctx_Pack.content_ctx.device_name.c_str());
							ImGui::Text(u8char("渲染时长: %f ms"), Ctx_Pack.content_ctx.cur_frame_time);
							ImGui::Text(u8char("UpdateLayeredWindow: %f ms"), Ctx_Pack.content_ctx.update_layered_time);
							ImGui::Text(u8char("渲染线程闲置时长: %f ms"), Ctx_Pack.content_ctx.cur_frame_sleep_time);
							ImGui::Text(u8char("计算傅里叶时长: %f ms"), Ctx_Pack.content_ctx.fft_calc_time);
							ImGui::Text(u8char("音频处理线程闲置时长: %f ms"), Ctx_Pack.content_ctx.fft_sleep_time);
							ImGui::Text(u8char("捕获采样数: %d"), Ctx_Pack.content_ctx.samples_cap);
							ImGui::Text(u8char("渲染线程冲突: %d times"), Ctx_Pack.content_ctx.render_thread_aquire_fft_buffer_failed);
							ImGui::Text(u8char("音频线程冲突: %d times"), Ctx_Pack.content_ctx.audio_thread_aquire_fft_buffer_failed);
							ImGui::EndChild();
						}
						ImGui::EndTabItem();
					}
					if (ImGui::BeginTabItem(u8char("程序设置"), nullptr, ImGuiTabItemFlags_::ImGuiTabItemFlags_None)) {
						/*
						* 程序设置
						*/
						ImGui::EndTabItem();
					}
					ImGui::EndTabBar();
				}
				ImGui::End();
			}

			//ImGui::ShowDemoWindow();

			// Rendering
			ImGui::Render();
			Ctx_Pack.config_ctx.d3d_ctx.g_pd3dDeviceContext->OMSetRenderTargets(1, addr(Ctx_Pack.config_ctx.d3d_ctx.g_mainRenderTargetView), NULL);
			Ctx_Pack.config_ctx.d3d_ctx.g_pd3dDeviceContext->ClearRenderTargetView(Ctx_Pack.config_ctx.d3d_ctx.g_mainRenderTargetView, clear_color);
			ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
			Ctx_Pack.config_ctx.d3d_ctx.g_pSwapChain->Present(1, 0);  //Present with vsync
		} else {
			//窗口进后台，不需要如此频繁PeekMessage
			Sleep(123);
		}
	}
	#pragma endregion

	/*
	* 资源清理
	*/
	#pragma region MyRegion

	//TODO: 退出前设置信号量通知他线程关闭，否则容易出事

	Shell_NotifyIcon(NIM_DELETE, &notify_icon_data);

	#pragma endregion
}

LRESULT WINAPI Vifft_config_Proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam)) {
		return 0;
	}
		
	switch (msg) {
	case WM_SIZE:
		if (Ctx_Pack.config_ctx.d3d_ctx.g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED) {
			Ctx_Pack.config_ctx.d3d_ctx.cleanup_render_target();
			Ctx_Pack.config_ctx.d3d_ctx.g_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
			Ctx_Pack.config_ctx.d3d_ctx.create_render_target();
		}
		return 0;
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
			return 0;
		break;
	case WM_CLOSE:
		//隐藏
		ShowWindow(hwnd, SW_HIDE);
		return 0;
	case WM_DESTROY:
		//退出程序
		PostQuitMessage(0);
		return 0;
	case WM_USER:
	{
		if (lParam == WM_RBUTTONUP) {
			POINT pt;
			GetCursorPos(&pt);
			TrackPopupMenuEx(
				h_notify_icon_menu,
				TPM_RIGHTALIGN | TPM_BOTTOMALIGN | TPM_NOANIMATION | TPM_LEFTBUTTON,
				pt.x,
				pt.y,
				vifft_config_wnd,
				nullptr
			);
		} else if (lParam == WM_LBUTTONUP) {
			ShowWindow(vifft_config_wnd, SW_SHOW);
			SetForegroundWindow(vifft_config_wnd);
		}
		return 0;
	}
	case WM_COMMAND:
	{
		if (wParam == 0) {
			if (hwnd == vifft_config_wnd) {
				DestroyWindow(hwnd);
			}
		} else if (wParam == 1) {
			MessageBox(hwnd, L"趁早转行, 为时不晚\ngithub.com/SuricSun", L"关于", MB_OK | MB_TOPMOST);
		}
		return 0;
	}
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

BOOL CreateSystemTrayAndMenu(HWND hwnd_that_the_tray_belongs) {

	h_notify_icon_menu = CreatePopupMenu();
	if (h_notify_icon_menu == NULL) {
		return FALSE;
	}
	AppendMenuW(h_notify_icon_menu, MF_STRING, 0, L"退出");
	AppendMenuW(h_notify_icon_menu, MF_SEPARATOR, 0, NULL);
	AppendMenuW(h_notify_icon_menu, MF_STRING, 1, L"关于");

	// * 创建托盘
	notify_icon_data = {};
	ZeroMemory(&notify_icon_data, sizeof(notify_icon_data));
	notify_icon_data.cbSize = sizeof(notify_icon_data);
	notify_icon_data.hWnd = hwnd_that_the_tray_belongs;
	notify_icon_data.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP; // TIP是hover提示 MESSAGE是点击托盘图标会发送消息到消息队列
	notify_icon_data.hIcon = main_icon;
	notify_icon_data.uID = (UINT)hwnd_that_the_tray_belongs;
	notify_icon_data.uCallbackMessage = UINT(WM_USER);
	notify_icon_data.uVersion = NOTIFYICON_VERSION;
	CopyMemory(notify_icon_data.szTip, L"Vifft 2022\n正在运行", sizeof(L"Vifft 2022\n正在运行"));

	if (Shell_NotifyIconW(NIM_ADD, &notify_icon_data) == FALSE) {
		return FALSE;
	}

	return TRUE;
}

const ImWchar* GetMyGlyphRange() {

	static const ImWchar ranges[] =
	{
		0x0020, 0x00FF, // Basic Latin + Latin Supplement
		0x2000, 0x206F, // General Punctuation
		0x3000, 0x30FF, // CJK Symbols and Punctuations, Hiragana, Katakana
		0x31F0, 0x31FF, // Katakana Phonetic Extensions
		0xFF00, 0xFFEF, // Half-width characters
		0xFFFD, 0xFFFD, // Invalid
		0x4e00, 0x9FAF, // CJK Ideograms
		0x2460, 0x2473, // Add by SCY, support circled arab number 1 to 20
		0,
	};
	return &ranges[0];
}