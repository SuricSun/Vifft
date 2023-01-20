#pragma once

#include"__Renderer_Common.h"

#include<Windows.h>
#include<dxgi.h>
#include<d3d11.h>
#include<DirectXMath.h>

#pragma comment(lib, "dxgi")
#pragma comment(lib, "d3d11")

namespace Suancai {

	namespace Graphics {

		namespace Renderer {

			using namespace DirectX;

			class D3D11_Renderer {
			public:
				ID3D11Device* p_device = NULL;
				ID3D11DeviceContext* p_ctx = NULL;
			public:
				D3D11_Renderer();
				BOOL init();
				~D3D11_Renderer();
			};

			class Graphics_Object {
			public:
				class Transform {
				public:
					XMFLOAT4X4A t;
				};
			public:
				D3D11_Renderer* p_parent_renderer = NULL;
				ID3D11Buffer* p_gpu_index_buffer = NULL;
				ID3D11Buffer* p_gpu_vertex_buffer = NULL;
				ID3D11Buffer* p_gpu_constant_buffer = NULL;
			};
		}
	}
}