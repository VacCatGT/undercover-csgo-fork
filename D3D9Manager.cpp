#include "D3D9Manager.h"
#include <d3dx9shader.h>
#include "D3D9Shaders.h"

using namespace util::draw;

constexpr auto D3DFVF_CUSTOM = (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1);

struct d3d9_vertex
{
	float pos[ 3 ];
	D3DCOLOR col;
	float uv[ 2 ];
};

// This backups shader constant that are overwritten with the cheats
// to prevent csgo's vertex shaders from breaking (displacements)
template<typename _type, bool pixel_shader = false>
struct shader_constant
{
	shader_constant(IDirect3DDevice9 *device_ptr, LPD3DXCONSTANTTABLE const_table, const char *var_name)
	{
		D3DXCONSTANT_DESC const_desc;
		UINT count;
		const auto const_handle = const_table->GetConstantByName(nullptr, var_name);
		const_table->GetConstantDesc(const_handle, &const_desc, &count);
		_idx    = const_desc.RegisterIndex;
		_device = device_ptr;

		if constexpr (pixel_shader)
			_device->GetPixelShaderConstantF(_idx, _bak.data(), _bak.size() / 4);
		else
			_device->GetVertexShaderConstantF(_idx, _bak.data(), _bak.size() / 4);
	}

	~shader_constant()
	{
		if constexpr (pixel_shader)
			_device->SetPixelShaderConstantF(_idx, _bak.data(), _bak.size() / 4);
		else
			_device->SetVertexShaderConstantF(_idx, _bak.data(), _bak.size() / 4);
	}

	void set(_type *val)
	{
		if constexpr (pixel_shader)
			_device->SetPixelShaderConstantF(_idx, reinterpret_cast<float*>(val), _bak.size() / 4);
		else
			_device->SetVertexShaderConstantF(_idx, reinterpret_cast<float*>(val), _bak.size() / 4);
	}

protected:
	std::array<float, sizeof(_type) / sizeof(float)> _bak;
	uint32_t _idx;
	IDirect3DDevice9 *_device;
};

void d3d9_manager::setup_shader()
{
	if (!_r.pixel_shader)
	{
		LPD3DXBUFFER code;
		LPD3DXBUFFER buffer_errors = nullptr;
		auto res                   = D3DXCompileShader(shaders::pixel_shader,
		                                               strlen(shaders::pixel_shader),
		                                               nullptr,
		                                               nullptr,
		                                               "main",
		                                               "ps_3_0",
		                                               0,
		                                               &code,
		                                               &buffer_errors,
		                                               &_r.constant_table);
		if (res == D3D_OK)
		{
			res = _device_ptr->CreatePixelShader((DWORD*)code->GetBufferPointer(), &_r.pixel_shader);
			code->Release();
		}
		else
		{
			LPVOID compile_errors = buffer_errors->GetBufferPointer();
			MessageBoxA(NULL,
			            (const char*)compile_errors,
			            "Pixel Shader Compile Error",
			            MB_OK | MB_ICONEXCLAMATION);
		}
	}

	if (!_r.vertex_shader)
	{
		LPD3DXBUFFER code;
		LPD3DXBUFFER buffer_errors = nullptr;

		D3DVERTEXELEMENT9 declaration[ ] =
		{
			{0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
			{0, 12, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0},
			{0, 16, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
			D3DDECL_END()
		};
		_device_ptr->CreateVertexDeclaration(declaration, &_r.vertex_declaration);

		auto res = D3DXCompileShader(shaders::vertex_shader,
		                             strlen(shaders::vertex_shader),
		                             nullptr,
		                             nullptr,
		                             "main",
		                             "vs_2_0",
		                             0,
		                             &code,
		                             &buffer_errors,
		                             &_r.vtx_constant_table);
		if (res == D3D_OK)
		{
			res = _device_ptr->CreateVertexShader((DWORD*)code->GetBufferPointer(), &_r.vertex_shader);
			code->Release();
		}
		else
		{
			LPVOID compile_errors = buffer_errors->GetBufferPointer();
			MessageBoxA(NULL,
			            (const char*)compile_errors,
			            "Vertex Shader Compile Error",
			            MB_OK | MB_ICONEXCLAMATION);
		}
	}

	if (!_r.key_shader)
	{
		LPD3DXBUFFER code;
		LPD3DXBUFFER buffer_errors = nullptr;
		auto res                   = D3DXCompileShader(shaders::key_shader,
		                                               strlen(shaders::key_shader),
		                                               nullptr,
		                                               nullptr,
		                                               "main",
		                                               "ps_3_0",
		                                               0,
		                                               &code,
		                                               &buffer_errors,
		                                               &_r.key_constant_table);
		if (res == D3D_OK)
		{
			res = _device_ptr->CreatePixelShader((DWORD*)code->GetBufferPointer(), &_r.key_shader);
			code->Release();
		}
		else
		{
			LPVOID compile_errors = buffer_errors->GetBufferPointer();
			MessageBoxA(NULL,
			            (const char*)compile_errors,
			            "Pixel Shader Compile Error",
			            MB_OK | MB_ICONEXCLAMATION);
		}
	}
}

bool d3d9_manager::setup_draw_state()
{
	_device_ptr->SetRenderState(D3DRS_COLORWRITEENABLE, 0xFFFFFFFF);
	_device_ptr->SetRenderState(D3DRS_SRGBWRITEENABLE, false);
	_device_ptr->SetSamplerState(0ul, D3DSAMP_ADDRESSU, D3DTADDRESS_MIRROR);
	_device_ptr->SetSamplerState(0ul, D3DSAMP_ADDRESSV, D3DTADDRESS_MIRROR);
	_device_ptr->SetSamplerState(0ul, D3DSAMP_ADDRESSW, D3DTADDRESS_MIRROR);
	_device_ptr->SetSamplerState(0ul, D3DSAMP_SRGBTEXTURE, 0ul);

	_device_ptr->GetVertexDeclaration(&_bak.vert_declaration);

	if (_device_ptr->CreateStateBlock(D3DSBT_PIXELSTATE, &_r.state_block) != D3D_OK)
		return false;

	_device_ptr->GetTexture(0, &_bak.texture);
	_device_ptr->GetVertexShader(&_bak.vert_shader);
	_device_ptr->GetPixelShader(&_bak.pixel_shader);
	_device_ptr->SetTexture(0, nullptr);
	_device_ptr->GetFVF(&_bak.fvf);
	_device_ptr->SetFVF(D3DFVF_CUSTOM);
	_device_ptr->SetStreamSource(0, _r.vtx_buffer, 0, sizeof(d3d9_vertex));
	_device_ptr->SetIndices(_r.idx_buffer);


	_device_ptr->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, TRUE);
	_device_ptr->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	_device_ptr->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
	_device_ptr->SetRenderState(D3DRS_LIGHTING, false);
	_device_ptr->SetRenderState(D3DRS_ZENABLE, false);
	_device_ptr->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	_device_ptr->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	_device_ptr->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	_device_ptr->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	_device_ptr->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	_device_ptr->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
	_device_ptr->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	_device_ptr->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);

	_device_ptr->SetRenderState(D3DRS_SCISSORTESTENABLE, true);
	_device_ptr->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	_device_ptr->SetRenderState(D3DRS_ALPHATESTENABLE, false);
	_device_ptr->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	_device_ptr->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	_device_ptr->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	//Backup Transformation matrices
	_device_ptr->GetTransform(D3DTS_WORLD, &_bak.world);
	_device_ptr->GetTransform(D3DTS_VIEW, &_bak.view);
	_device_ptr->GetTransform(D3DTS_PROJECTION, &_bak.projection);

	{
		float L                = 0.5f;
		float R                = 9;
		float T                = 0.5f;
		float B                = 9;
		D3DMATRIX mat_identity = {
			{1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f}
		};
		D3DMATRIX mat_projection =
		{
			2.0f / (R - L),
			0.0f,
			0.0f,
			0.0f,
			0.0f,
			2.0f / (T - B),
			0.0f,
			0.0f,
			0.0f,
			0.0f,
			0.5f,
			0.0f,
			(L + R) / (L - R),
			(T + B) / (B - T),
			0.5f,
			1.0f,
		};
		_device_ptr->SetTransform(D3DTS_WORLD, &mat_identity);
		_device_ptr->SetTransform(D3DTS_VIEW, &mat_identity);
		_device_ptr->SetTransform(D3DTS_PROJECTION, &mat_projection);
	}

	return true;
}

void d3d9_manager::destroy_draw_state()
{
	_device_ptr->SetTransform(D3DTS_WORLD, &_bak.world);
	_device_ptr->SetTransform(D3DTS_VIEW, &_bak.view);
	_device_ptr->SetTransform(D3DTS_PROJECTION, &_bak.projection);

	if (_r.state_block)
	{
		_r.state_block->Apply();
		_r.state_block->Release();
		_r.state_block = nullptr;
	}

	_device_ptr->SetVertexDeclaration(_bak.vert_declaration);
	_device_ptr->SetPixelShader(_bak.pixel_shader);
	_device_ptr->SetVertexShader(_bak.vert_shader);
	_device_ptr->SetTexture(0, _bak.texture);
	_device_ptr->SetFVF(_bak.fvf);
}

void d3d9_manager::save_csgo_state()
{
	const auto device = _device_ptr;
	device->GetRenderState(D3DRS_COLORWRITEENABLE, &_color_write_enable);
	device->GetVertexDeclaration(reinterpret_cast<IDirect3DVertexDeclaration9 **>(&_vertex_declaration));
	device->GetVertexShader(reinterpret_cast<IDirect3DVertexShader9 **>(&_vertex_shader));

	device->GetSamplerState(0ul, D3DSAMP_ADDRESSU, &_sampler_u);
	device->GetSamplerState(0ul, D3DSAMP_ADDRESSV, &_sampler_v);
	device->GetSamplerState(0ul, D3DSAMP_ADDRESSW, &_sampler_w);
	device->GetSamplerState(0ul, D3DSAMP_SRGBTEXTURE, &_sampler_srgb);
}

void d3d9_manager::apply_csgo_state()
{
	const auto device = _device_ptr;
	device->SetSamplerState(0ul, D3DSAMP_SRGBTEXTURE, _sampler_srgb);
	device->SetSamplerState(0ul, D3DSAMP_ADDRESSW, _sampler_w);
	device->SetSamplerState(0ul, D3DSAMP_ADDRESSV, _sampler_v);
	device->SetSamplerState(0ul, D3DSAMP_ADDRESSU, _sampler_u);

	device->SetRenderState(D3DRS_COLORWRITEENABLE, _color_write_enable);
	device->SetRenderState(D3DRS_SRGBWRITEENABLE, true);
	device->SetVertexDeclaration(reinterpret_cast<IDirect3DVertexDeclaration9 *>(_vertex_declaration));
	device->SetVertexShader(reinterpret_cast<IDirect3DVertexShader9 *>(_vertex_shader));
}


bool d3d9_manager::create_font_texture()
{


	return true;
}

bool d3d9_manager::create_device_objects()
{
	return false;
}

void d3d9_manager::invalidate_shader()
{
	if (_r.pixel_shader)
	{
		_r.pixel_shader->Release();
		_r.pixel_shader = nullptr;
	}
	if (_r.constant_table)
	{
		_r.constant_table->Release();
		_r.constant_table = nullptr;
	}
	if (_r.vertex_shader)
	{
		_r.vertex_shader->Release();
		_r.vertex_shader = nullptr;
	}
	if (_r.vertex_declaration)
	{
		_r.vertex_declaration->Release();
		_r.vertex_declaration = nullptr;
	}
	if (_r.vtx_constant_table)
	{
		_r.vtx_constant_table->Release();
		_r.vtx_constant_table = nullptr;
	}
	if (_r.key_shader)
	{
		_r.key_shader->Release();
		_r.key_shader = nullptr;
	}
	if (_r.key_constant_table)
	{
		_r.key_constant_table->Release();
		_r.key_constant_table = nullptr;
	}
}


bool d3d9_manager::invalidate_device_objects()
{
	if (!_device_ptr)
		return false;
	if (_r.vtx_buffer)
	{
		_r.vtx_buffer->Release();
		_r.vtx_buffer = nullptr;
	}
	if (_r.idx_buffer)
	{
		_r.idx_buffer->Release();
		_r.idx_buffer = nullptr;
	}
	if (_r.font_texture)
		_r.font_texture->Release();
	if (_r.state_block)
	{
		_r.state_block->Release();
		_r.state_block = nullptr;
	}
	//invalidate_shader( );

	if (_r.buffer_copy)
	{
		_r.buffer_copy->Release();
		_r.buffer_copy = nullptr;
	}
	if (_r.blur_copy)
	{
		_r.blur_copy->Release();
		_r.blur_copy = nullptr;
	}

	_r.font_texture = nullptr;
	_tex_dict.pre_reset();

	return true;
}


// directx seems to be using RABG internally, maybe not, this is weird
void copy_convert(const uint8_t *rgba, uint8_t *out, const size_t size)
{
	auto in  = reinterpret_cast<const uint32_t*>(rgba);
	auto buf = reinterpret_cast<uint32_t*>(out);
	for (auto i = 0u; i < (size / 4); ++i)
	{
		const auto pixel = *in++;
		*buf++           = (pixel & 0xFF00FF00) | ((pixel & 0xFF0000) >> 16) | ((pixel & 0xFF) << 16);
	}
}

