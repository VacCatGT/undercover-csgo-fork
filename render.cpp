#include "undercover.h"

#define strdup _strdup

namespace render {
	Font esp;;
	Font pixel;;
	Font console;;
	Font hud;;
	Font cs;;
	Font indicator;;
}

void render::init( ) {

	D3DXCreateFont( g_csgo.m_device, 12, 0, 200, -1, false, DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, ANTIALIASED_QUALITY, VARIABLE_PITCH, XOR( "Josefin Sans" ), &esp.m_handle );
	D3DXCreateFont( g_csgo.m_device, 9, 0, 200, 0, false, DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, NONANTIALIASED_QUALITY, DEFAULT_PITCH, XOR( "Small Fonts" ), &pixel.m_handle );
	D3DXCreateFont( g_csgo.m_device, 10, 0, 0, 0, false, DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, NONANTIALIASED_QUALITY, DEFAULT_PITCH, XOR( "Lucida Console" ), &console.m_handle );
	D3DXCreateFont( g_csgo.m_device, 12, 0, 0, 0, false, DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, NONANTIALIASED_QUALITY, DEFAULT_PITCH, XOR( "Verdana" ), &hud.m_handle );
	D3DXCreateFont( g_csgo.m_device, 28, 0, 0, 0, false, DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, NONANTIALIASED_QUALITY, DEFAULT_PITCH, XOR( "Counter-Strike" ), &cs.m_handle );

	g_csgo.m_engine->GetScreenSize( g_cl.m_width, g_cl.m_height );
}

bool render::WorldToScreen( const vec3_t& world, vec2_t& screen ) {
	float w;

	const VMatrix& matrix = g_csgo.m_engine->WorldToScreenMatrix( );

	// check if it's in view first.
	// note - dex; w is below 0 when world position is around -90 / +90 from the player's camera on the y axis.
	w = matrix[ 3 ][ 0 ] * world.x + matrix[ 3 ][ 1 ] * world.y + matrix[ 3 ][ 2 ] * world.z + matrix[ 3 ][ 3 ];
	if( w < 0.001f )
		return false;

	// calculate x and y.
	screen.x = matrix[ 0 ][ 0 ] * world.x + matrix[ 0 ][ 1 ] * world.y + matrix[ 0 ][ 2 ] * world.z + matrix[ 0 ][ 3 ];
	screen.y = matrix[ 1 ][ 0 ] * world.x + matrix[ 1 ][ 1 ] * world.y + matrix[ 1 ][ 2 ] * world.z + matrix[ 1 ][ 3 ];

	screen /= w;

	// calculate screen position.
	screen.x = ( g_cl.m_width / 2 ) + ( screen.x * g_cl.m_width ) / 2;
	screen.y = ( g_cl.m_height / 2 ) - ( screen.y * g_cl.m_height ) / 2;

	return true;
}

bool render::WorldToScreen3d( const vec3_t& world, vec3_t& screen ) {
	float w;

	const VMatrix& matrix = g_csgo.m_engine->WorldToScreenMatrix( );

	// check if it's in view first.
	// note - dex; w is below 0 when world position is around -90 / +90 from the player's camera on the y axis.
	w = matrix[ 3 ][ 0 ] * world.x + matrix[ 3 ][ 1 ] * world.y + matrix[ 3 ][ 2 ] * world.z + matrix[ 3 ][ 3 ];
	if ( w < 0.001f )
		return false;

	// calculate x and y.
	screen.x = matrix[ 0 ][ 0 ] * world.x + matrix[ 0 ][ 1 ] * world.y + matrix[ 0 ][ 2 ] * world.z + matrix[ 0 ][ 3 ];
	screen.y = matrix[ 1 ][ 0 ] * world.x + matrix[ 1 ][ 1 ] * world.y + matrix[ 1 ][ 2 ] * world.z + matrix[ 1 ][ 3 ];

	screen /= w;

	// calculate screen position.
	screen.x = ( g_cl.m_width / 2 ) + ( screen.x * g_cl.m_width ) / 2;
	screen.y = ( g_cl.m_height / 2 ) - ( screen.y * g_cl.m_height ) / 2;

	return true;
}

void render::line( vec2_t v0, vec2_t v1, Color color ) {
	const auto col = D3DCOLOR_ARGB( color.a( ), color.r( ), color.g( ), color.b( ) );
	SD3DVertex pVertex[ 2 ] = { { v0.x, v0.y, 0.0f, 1.0f, col }, { v1.x, v1.y, 0.0f, 1.0f, col } };
	g_csgo.m_device->DrawPrimitiveUP( D3DPT_LINELIST, 1, &pVertex, sizeof( SD3DVertex ) );
}

void render::world_circle( vec3_t origin, float radius, Color color, bool thick ) {
	//vec2_t previous_screen_pos, screen_pos;

	//g_csgo.m_surface->DrawSetColor( color );
	//float step = M_PI * 2.0 / 2047;

	//for( float rotation = 0; rotation < ( M_PI * 2.0 ); rotation += step ) {
	//	vec3_t pos( radius * cos( rotation ) + origin.x, radius * sin( rotation ) + origin.y, origin.z );

	//	if( render::WorldToScreen( pos, screen_pos ) ) {
	//		if( previous_screen_pos.valid( ) && screen_pos.valid( ) && previous_screen_pos != screen_pos ) {
	//			g_csgo.m_surface->DrawLine( previous_screen_pos.x, previous_screen_pos.y, screen_pos.x, screen_pos.y );
	//			if( thick ) {
	//				g_csgo.m_surface->DrawLine( previous_screen_pos.x - 1, previous_screen_pos.y - 1, screen_pos.x + 1, screen_pos.y + 1 );
	//			}
	//		}
	//		previous_screen_pos = screen_pos;
	//	}
	//}
}

void render::filled_world_circle( vec3_t origin, float radius, Color color ) {
	//vec2_t center;
	//render::WorldToScreen( origin, center );

	//for( int degrees = 0; degrees < 20; degrees++ ) {
	//	vec2_t old_point, cur_point;

	//	if( origin.z == 0 ) {
	//		cur_point = { origin.x + sin( math::rad_to_deg( degrees * 18 ) ) * radius, origin.y + cos( math::rad_to_deg( degrees * 18 ) ) * radius };    
	//		old_point = { origin.x + sin( math::rad_to_deg( degrees * 18 - 18 ) ) * radius, origin.y + cos( math::rad_to_deg( degrees * 18 - 18 ) ) * radius };
	//		triangle_filled( { vec2_t{ cur_point.x, cur_point.y }, vec2_t{ old_point.x, old_point.y }, vec2_t{ center.x, center.y } } , color );
	//	}
	//	else {
	//		if ( render::WorldToScreen( { origin.x + sin( math::deg_to_rad( degrees * 18 ) ) * radius, origin.y + cos( math::deg_to_rad( degrees * 18 ) ) * radius, origin.z }, cur_point )
	//		&& render::WorldToScreen( { origin.x + sin( math::deg_to_rad( degrees * 18 - 18 ) ) * radius, origin.y + cos( math::deg_to_rad( degrees * 18 - 18 ) ) * radius, origin.z }, old_point ) )
	//			triangle_filled( { vec2_t{ cur_point.x, cur_point.y }, vec2_t{ old_point.x, old_point.y }, vec2_t{ center.x, center.y } } , color );
	//	}
	//}
}

void render::line( int x0, int y0, int x1, int y1, Color color ) {
	const auto col = D3DCOLOR_ARGB( color.a( ), color.r( ), color.g( ), color.b( ) );
	SD3DVertex pVertex[ 2 ] = { { x0, y0, 0.0f, 1.0f, col }, { x1, y1, 0.0f, 1.0f, col } };
	g_csgo.m_device->DrawPrimitiveUP( D3DPT_LINELIST, 1, &pVertex, sizeof( SD3DVertex ) );
}

void render::rect( int x, int y, int w, int h, Color color ) {
	render::rect_filled( x, y, w, 1, color );
	render::rect_filled( x, y, 1, h, color );
	render::rect_filled( x + w - 1, y, 1, h, color );
	render::rect_filled( x, y + h - 1, w, 1, color );
}

void render::rect_filled( int x, int y, int w, int h, Color color ) {
	const auto col = D3DCOLOR_ARGB( color.a( ), color.r( ), color.g( ), color.b( ) );
	render::SD3DVertex pVertex[ 4 ] = { { x, y + h, 0.0f, 1.0f, col }, { x, y, 0.0f, 1.0f, col }, { x + w, y + h, 0.0f, 1.0f, col }, { x + w, y, 0.0f, 1.0f, col } };
	g_csgo.m_device->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, pVertex, sizeof( SD3DVertex ) );
}

void render::rect_filled_fade( int x, int y, int w, int h, Color color, int a1, int a2 ) {
	g_csgo.m_surface->DrawSetColor( color );
	g_csgo.m_surface->DrawFilledRectFade( x, y, x + w, y + h, a1, a2, false );
}

void render::setup_render_state( ) {

	//g_csgo.m_device->SetVertexShader( nullptr );
	g_csgo.m_device->SetPixelShader( nullptr );
	g_csgo.m_device->SetFVF( D3DFVF_XYZRHW | D3DFVF_DIFFUSE ); // NOLINT
	g_csgo.m_device->SetRenderState( D3DRS_LIGHTING, FALSE );
	g_csgo.m_device->SetRenderState( D3DRS_FOGENABLE, FALSE );
	g_csgo.m_device->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	g_csgo.m_device->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );

	g_csgo.m_device->SetRenderState( D3DRS_ZENABLE, D3DZB_FALSE );
	g_csgo.m_device->SetRenderState( D3DRS_SCISSORTESTENABLE, TRUE );
	g_csgo.m_device->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
	g_csgo.m_device->SetRenderState( D3DRS_STENCILENABLE, FALSE );

	g_csgo.m_device->SetRenderState( D3DRS_MULTISAMPLEANTIALIAS, FALSE );
	g_csgo.m_device->SetRenderState( D3DRS_ANTIALIASEDLINEENABLE, FALSE );

	g_csgo.m_device->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	g_csgo.m_device->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
	g_csgo.m_device->SetRenderState( D3DRS_SEPARATEALPHABLENDENABLE, TRUE );
	g_csgo.m_device->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	g_csgo.m_device->SetRenderState( D3DRS_SRCBLENDALPHA, D3DBLEND_INVDESTALPHA );
	g_csgo.m_device->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
	g_csgo.m_device->SetRenderState( D3DRS_DESTBLENDALPHA, D3DBLEND_ONE );

	g_csgo.m_device->SetRenderState( D3DRS_SRGBWRITEENABLE, FALSE );
	g_csgo.m_device->SetRenderState( D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN // NOLINT NOLINTNEXTLINE
		| D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA );

	//g_csgo.m_device->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
	//g_csgo.m_device->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	//g_csgo.m_device->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	//g_csgo.m_device->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
	//g_csgo.m_device->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	//g_csgo.m_device->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );

	//g_csgo.m_device->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_NONE );
	//g_csgo.m_device->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_NONE );
}

void render::rect_outlined( int x, int y, int w, int h, Color color, Color color2 ) {
	rect( x, y, w, h, color );
	rect( x - 1, y - 1, w + 2, h + 2, color2 );
	rect( x + 1, y + 1, w - 2, h - 2, color2 );
}

void render::circle( int x, int y, int radius, int segments, Color color ) {
	/*static int texture = g_csgo.m_surface->CreateNewTextureID( true );

	g_csgo.m_surface->DrawSetTextureRGBA( texture, &colors::white, 1, 1 );
	g_csgo.m_surface->DrawSetColor( color );
	g_csgo.m_surface->DrawSetTexture( texture );

	std::vector< Vertex > vertices{ };

	float step = math::pi_2 / segments;
	for( float i{ 0.f }; i < math::pi_2; i += step )
		vertices.emplace_back( vec2_t{ x + ( radius * std::cos( i ) ), y + ( radius * std::sin( i ) ) } );

	g_csgo.m_surface->DrawTexturedPolygon( vertices.size( ), vertices.data( ) );*/
}
void render::gradient( int x, int y, int w, int h, Color color1, Color color2 ) {
	/*g_csgo.m_surface->DrawSetColor( color1 );
	g_csgo.m_surface->DrawFilledRectFade( x, y, x + w, y + h, color1.a( ), 0, false );

	g_csgo.m_surface->DrawSetColor( color2 );
	g_csgo.m_surface->DrawFilledRectFade( x, y, x + w, y + h, 0, color2.a( ), false );*/
}

void render::sphere( vec3_t origin, float radius, float angle, float scale, Color color ) {
	//std::vector< Vertex > vertices{ };

	//// compute angle step for input radius and precision.
	//float step = ( 1.f / radius ) + math::deg_to_rad( angle );

	//for( float lat{ }; lat < ( math::pi * scale ); lat += step ) {
	//	// reset.
	//	vertices.clear( );

	//	for( float lon{ }; lon < math::pi_2; lon += step ) {
	//		vec3_t point{
	//			origin.x + ( radius * std::sin( lat ) * std::cos( lon ) ),
	//			origin.y + ( radius * std::sin( lat ) * std::sin( lon ) ),
	//			origin.z + ( radius * std::cos( lat ) )
	//		};

	//		vec2_t screen;
	//		if( WorldToScreen( point, screen ) )
	//			vertices.emplace_back( screen );
	//	}

	//	if( vertices.empty( ) )
	//		continue;

	//	g_csgo.m_surface->DrawSetColor( color );
	//	g_csgo.m_surface->DrawTexturedPolyLine( vertices.size( ), vertices.data( ) );
	//}
}

Vertex render::RotateVertex( const vec2_t& p, const Vertex& v, float angle ) {
	// convert theta angle to sine and cosine representations.
	float c = std::cos( math::deg_to_rad( angle ) );
	float s = std::sin( math::deg_to_rad( angle ) );

	return {
		p.x + ( v.m_pos.x - p.x ) * c - ( v.m_pos.y - p.y ) * s,
		p.y + ( v.m_pos.x - p.x ) * s + ( v.m_pos.y - p.y ) * c
	};
}

void render::Font::string( int x, int y, Color color, const std::string& text, StringFlags_t flags /*= render::ALIGN_LEFT */ ) {
	wstring( x, y, color, strdup( text.c_str( ) ), flags );
}

void render::Font::string( int x, int y, Color color, const std::stringstream& text, StringFlags_t flags /*= render::ALIGN_LEFT */ ) {
//	wstring( x, y, color, text.str( ), flags );
}

void render::Font::wstring( int x, int y, Color color, char* text, StringFlags_t flags /*= render::ALIGN_LEFT */, bool outlined ) {

	auto size = this->size( text );
	
	if( flags & ALIGN_RIGHT )
		x -= size.m_width;
	if( flags & render::ALIGN_CENTER )
		x -= size.m_width / 2;
	
	const auto col = D3DCOLOR_ARGB( color.a( ), color.r( ), color.g( ), color.b( ) );

	va_list args;
	char cBuffer[ 256 ];

	va_start( args, text );
	vsprintf_s( cBuffer, text, args );
	va_end( args );

	RECT pRect;
	if ( outlined ) {	
		//pRect.left = x - 1;
		//pRect.top = y;
		//m_handle->DrawText( NULL, cBuffer, strlen( cBuffer ), &pRect, DT_NOCLIP, D3DCOLOR_RGBA( 0, 0, 0, ( int )( color.a( ) * 0.75f ) ) );
		pRect.left = x + 1;
		pRect.top = y;
		m_handle->DrawText( NULL, cBuffer, strlen( cBuffer ), &pRect, DT_NOCLIP, D3DCOLOR_RGBA( 0, 0, 0, ( int )( color.a( ) * 0.55f ) ) );
		//pRect.left = x;
		//pRect.top = y - 1;
		//m_handle->DrawText( NULL, cBuffer, strlen( cBuffer ), &pRect, DT_NOCLIP, D3DCOLOR_RGBA( 0, 0, 0, ( int )( color.a( ) * 0.75f ) ) );
		pRect.left = x;
		pRect.top = y + 1;
		m_handle->DrawText( NULL, cBuffer, strlen( cBuffer ), &pRect, DT_NOCLIP, D3DCOLOR_RGBA( 0, 0, 0, ( int )( color.a( ) * 0.55f ) ) );
	}
	pRect.left = x;
	pRect.top = y;
	m_handle->DrawText( NULL, cBuffer, strlen( cBuffer ), &pRect, DT_NOCLIP, col );
}

void render::triangle_filled( std::array<vec2_t, 4>points, Color color ) {
	const auto col = D3DCOLOR_ARGB( color.a( ), color.r( ), color.g( ), color.b( ) );

	render::SD3DVertex pVertex[ 4 ];
	pVertex[ 0 ] = { points.at( 0 ).x, points.at( 0 ).y, 0.0f, 1.0f, col };
	pVertex[ 1 ] = { points.at( 1 ).x, points.at( 1 ).y, 0.0f, 1.0f, col };
	pVertex[ 2 ] = { points.at( 2 ).x, points.at( 2 ).y, 0.0f, 1.0f, col };
	pVertex[ 3 ] = { points.at( 3 ).x, points.at( 2 ).y, 0.0f, 1.0f, col };

	g_csgo.m_device->DrawPrimitiveUP( D3DPT_TRIANGLEFAN, 2, pVertex, sizeof( SD3DVertex ) );
}

render::FontSize_t render::Font::size( const std::string& text ) {
	return wsize( util::MultiByteToWide( text ) );
}

render::FontSize_t render::Font::wsize( const std::wstring& text ) {
	char* string = strdup( util::WideToMultiByte( text ).c_str( ) );
	FontSize_t res;
	RECT pRect = RECT( );
	m_handle->DrawText( NULL, string, strlen( string ), &pRect, DT_CALCRECT, D3DCOLOR_RGBA( 0, 0, 0, 0 ) );
	res.m_width = pRect.right;
	res.m_height = 16;
	return res;
}

void render::arrow( vec2_t pos, Color fill_clr, Color outline_clr )
{
	
}