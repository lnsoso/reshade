#pragma once

#include "runtime.hpp"
#include "utils\critical_section.hpp"

#include <algorithm>
#include <d3d11_3.h>

namespace reshade
{
	namespace runtimes
	{
		class d3d11_runtime : public runtime
		{
		public:
			d3d11_runtime(ID3D11Device *device, IDXGISwapChain *swapchain);
			~d3d11_runtime();

			bool on_init(const DXGI_SWAP_CHAIN_DESC &desc);
			void on_reset() override;
			void on_reset_effect() override;
			void on_present() override;
			void on_draw_call(ID3D11DeviceContext *context, unsigned int vertices);
			void on_apply_effect() override;
			void on_apply_effect_technique(const technique *technique) override;
			void on_create_depthstencil_view(ID3D11Resource *resource, ID3D11DepthStencilView *depthstencil);
			void on_delete_depthstencil_view(ID3D11DepthStencilView *depthstencil);
			void on_set_depthstencil_view(ID3D11DepthStencilView *&depthstencil);
			void on_get_depthstencil_view(ID3D11DepthStencilView *&depthstencil);
			void on_clear_depthstencil_view(ID3D11DepthStencilView *&depthstencil);
			void on_copy_resource(ID3D11Resource *&dest, ID3D11Resource *&source);

			const std::vector<std::unique_ptr<technique>> &GetTechniques() const
			{
				return _techniques;
			}
			void enlarge_uniform_data_storage()
			{
				_uniform_data_storage.resize(_uniform_data_storage.size() + 128);
			}
			unsigned char *get_uniform_data_storage()
			{
				return _uniform_data_storage.data();
			}
			size_t get_uniform_data_storage_size() const
			{
				return _uniform_data_storage.size();
			}
			void add_texture(texture *x)
			{
				_textures.push_back(std::unique_ptr<texture>(x));
			}
			void add_uniform(uniform *x)
			{
				_uniforms.push_back(std::unique_ptr<uniform>(x));
			}
			void add_technique(technique *x)
			{
				_techniques.push_back(std::unique_ptr<technique>(x));
			}

			ID3D11Device *_device;
			ID3D11DeviceContext *_immediate_context;
			IDXGISwapChain *_swapchain;

			ID3D11Texture2D *_backbuffer_texture;
			ID3D11ShaderResourceView *_backbuffer_texture_srv[2];
			ID3D11RenderTargetView *_backbuffer_rtv[3];
			ID3D11ShaderResourceView *_depthstencil_texture_srv;
			std::vector<ID3D11SamplerState *> _effect_sampler_states;
			std::vector<ID3D11ShaderResourceView *> _effect_shader_resources;
			ID3D11Buffer *_constant_buffer;

		private:
			struct depth_source_info
			{
				UINT width, height;
				FLOAT drawcall_count, vertices_count;
			};

			void screenshot(unsigned char *buffer) const override;
			bool update_effect(const fx::nodetree &ast, const std::vector<std::string> &pragmas, std::string &errors) override;
			bool update_texture(texture *texture, const unsigned char *data, size_t size) override;

			void detect_depth_source();
			bool create_depthstencil_replacement(ID3D11DepthStencilView *depthstencil);

			bool _is_multisampling_enabled;
			DXGI_FORMAT _backbuffer_format;
			std::unique_ptr<class d3d11_stateblock> _stateblock;
			ID3D11Texture2D *_backbuffer, *_backbuffer_resolved;
			ID3D11DepthStencilView *_depthstencil, *_depthstencil_replacement;
			ID3D11Texture2D *_depthstencil_texture;
			ID3D11DepthStencilView *_default_depthstencil;
			std::unordered_map<ID3D11DepthStencilView *, depth_source_info> _depth_source_table;
			ID3D11VertexShader *_copy_vertex_shader;
			ID3D11PixelShader *_copy_pixel_shader;
			ID3D11SamplerState *_copy_sampler;
			utils::critical_section _cs;
			ID3D11RasterizerState *_effect_rasterizer_state;
		};
	}
}