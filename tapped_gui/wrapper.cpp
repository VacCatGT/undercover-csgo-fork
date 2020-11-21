#include "../tapped.h"

MenuFramework g_frw{ };

int MenuFramework::RenderSecondaryHeader( int& subtab, const std::vector<std::string> subtab_names, float padding )
{
	ImColor c = ImColor( 0, 0, 0 );

	ImGui::PushFont( g_menu.droid_sans_bold );
	ImGui::PushStyleVar( ImGuiStyleVar_ItemSpacing, ImVec2( 0, 0 ) );

	ImGui::BeginChild( XOR( "subtabs" ), ImVec2( ImGui::GetWindowWidth( ), 35.f ), false, ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar );
	{
		ImGui::Dummy( ImVec2( 20, 0 ) );  ImGui::SameLine( 0, 0 );

		for ( int i = 0; i < subtab_names.size( ); i++ ) {
			if ( ImGui::ButtonT( subtab_names.at( i ).c_str( ), ImVec2( padding, 35.f ), subtab, i, c, false ) ) subtab = i; ImGui::SameLine( 0, 0 );
		}

	}
	ImGui::EndChild( );
	ImGui::PopFont( );
	ImGui::PopStyleVar( );

	return subtab;
}

void MenuFramework::DrawBackdrop( )
{
	ImVec2 p = ImGui::GetWindowPos( );

	ImGui::GetWindowDrawList( )->AddRectFilled( ImVec2( p.x + 1, p.y + 1 ), ImVec2( p.x + ImGui::GetWindowWidth( ) - 1, p.y + ImGui::GetWindowHeight( ) - 1 ), ImColor( 25, 31, 36, 255 ) );
	ImGui::GetWindowDrawList( )->AddImage( g_menu.tapped_background, ImVec2( p.x + 1, +p.y + 108 ), ImVec2( p.x + 1 + 798, +p.y + 108 + 541 ), ImVec2( 0, 0 ), ImVec2( 1, 1 ), ImColor( 255, 255, 255, 150 ) );
	ImGui::GetWindowDrawList( )->AddRectFilled( ImVec2( p.x, p.y + 19 ), ImVec2( p.x + ImGui::GetWindowWidth( ), p.y + 19 + 53 ), ImColor( 47, 59, 73, 255 ) );
	ImGui::GetWindowDrawList( )->AddRectFilled( ImVec2( p.x, p.y + 71 ), ImVec2( p.x + ImGui::GetWindowWidth( ), p.y + 19 + 53 + 35 ), ImColor( 21, 24, 25, 255 ) );
	ImGui::GetWindowDrawList( )->AddRectFilled( ImVec2( p.x, p.y + 71 + 36 ), ImVec2( p.x + ImGui::GetWindowWidth( ), p.y + 19 + 53 + 36 ), ImColor( 79, 79, 79, 255 ) );
	ImGui::GetWindowDrawList( )->AddRectFilled( ImVec2( p.x + 17, p.y + 126 ), ImVec2( p.x + ImGui::GetWindowWidth( ) - 16, p.y + ImGui::GetWindowHeight( ) - 16 ), ImColor( 62, 62, 62, 255 ) );
	ImGui::GetWindowDrawList( )->AddRectFilled( ImVec2( p.x + 18, p.y + 127 ), ImVec2( p.x + ImGui::GetWindowWidth( ) - 17, p.y + ImGui::GetWindowHeight( ) - 17 ), ImColor( 34, 40, 47, 255 ) );
}

int MenuFramework::RenderTabBar( int& page, const std::vector<const char*> tabs )
{
	ImColor c = ImColor( 0, 0, 0 ); ImVec2 p = ImGui::GetWindowPos( );
	ImGui::PushStyleVar( ImGuiStyleVar_ItemSpacing, ImVec2( 0, 0 ) );
	ImGui::Dummy( ImVec2( 0, 19 ) );
	ImGui::PushFont( g_menu.agency );
	ImGui::Dummy( ImVec2( ImGui::GetWindowWidth( ) / 2 - 196, 0 ) ); ImGui::SameLine( 0, 0 );
	ImGui::BeginChild( XOR( "Tabs" ), ImVec2( 400, 53.f ), false, ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar );
	{
		if ( ImGui::ButtonT_Filled( tabs.at( 0 ), ImVec2( 80.f, 52.f ), page, tabs::tab_one, c, false ) ) page = tabs::tab_one; ImGui::SameLine( 0, 0 );
		if ( ImGui::ButtonT_Filled( tabs.at( 1 ), ImVec2( 80.f, 52.f ), page, tabs::tab_two, c, false ) ) page = tabs::tab_two; ImGui::SameLine( 0, 7 + 58 + 7 + 6 );
		ImGui::GetWindowDrawList( )->AddImage( g_menu.tapped_logo, ImVec2( p.x + 371, p.y + 22 ), ImVec2( p.x + 371 + 58, p.y + 22 + 46 ), ImVec2( 0, 0 ), ImVec2( 1, 1 ), ImColor( 255, 255, 255, 255 ) );
		if ( ImGui::ButtonT_Filled( tabs.at( 2 ), ImVec2( 80.f, 52.f ), page, tabs::tab_three, c, false ) ) page = tabs::tab_three; ImGui::SameLine( 0, 0 );
		if ( ImGui::ButtonT_Filled( tabs.at( 3 ), ImVec2( 80.f, 52.f ), page, tabs::tab_four, c, false ) ) page = tabs::tab_four; ImGui::SameLine( 0, 0 );
	}
	ImGui::EndChild( );
	ImGui::PopFont( );
	ImGui::PopStyleVar( );

	return page;
}

void MenuFramework::StartWindow( )
{
	ImGui::SetNextWindowPos( ImVec2( 560, 215 ), ImGuiCond_::ImGuiCond_Once );
	ImGui::SetNextWindowSize( ImVec2( 800, 650 ), ImGuiCond_Always );
	ImGui::Begin( XOR( "Tapped.cc" ), &g_menu.m_opened, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize );
}

void MenuFramework::EndWindow( )
{
	ImGui::End( );
}


void MenuFramework::RenderModule( positions position, sizes size, std::string title ) {
	ImVec2 p = ImGui::GetWindowPos( );

	bool is_top = position == positions::center_top || position == positions::left_top || position == positions::right_top;

	bool is_half = size == sizes::half;

	int subraction_due_to_spacing = is_half && is_top ? 16 : 0;
	int countered_spacing_value = is_half && !is_top ? -1 : 0;

	switch ( position ) {
	case left_top:
		module_position = ImVec2( p.x + 38, p.y + 146 ); break;
	case left_bottom:
		module_position = ImVec2( p.x + 38, p.y + 382 ); break;
	case center_top:
		module_position = ImVec2( p.x + 289, p.y + 146 ); break;
	case center_bottom:
		module_position = ImVec2( p.x + 289, p.y + 382 ); break;
	case right_top:
		module_position = ImVec2( p.x + 540, p.y + 146 ); break;
	case right_bottom:
		module_position = ImVec2( p.x + 540, p.y + 382 ); break;
	default: module_position = ImVec2( p.x + 265, p.y + 147 ); break;
	}
	module_name = title;

	if ( is_top )
		ImGui::Dummy( ImVec2( 0, 32 ) );
	else
		ImGui::Dummy( ImVec2( 0, 1 ) );

	ImGui::Dummy( ImVec2( 0, 0 ) );

	if ( position == positions::left_top || position == positions::left_bottom )
		ImGui::SameLine( 35, 0 );
	if ( position == positions::center_top || position == positions::center_bottom )
		ImGui::SameLine( 14, 0 );
	if ( position == positions::right_top || position == positions::right_bottom )
		ImGui::SameLine( -2, 0 );

	ImGui::GetWindowDrawList( )->AddRectFilled( ImVec2( module_position.x - 1, module_position.y - 1 ), ImVec2( module_position.x + 221, module_position.y + ( 470 / size ) - subraction_due_to_spacing + 1 + countered_spacing_value ), ImColor( 56, 59, 62, 255 ) );
	ImGui::GetWindowDrawList( )->AddRectFilled( module_position, ImVec2( module_position.x + 220, module_position.y + ( 469 / size ) - subraction_due_to_spacing + 1 + countered_spacing_value ), ImColor( 31, 37, 43, 255 ) );
	ImGui::PushFont( g_menu.droid_sans );
	ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 16, 16 ) );
	ImGui::BeginChild( title.c_str( ), ImVec2( 240, ( ( 468 / size ) - subraction_due_to_spacing + countered_spacing_value ) ), false, ImGuiWindowFlags_NoCollapse /*| ImGuiWindowFlags_NoScrollbar*/ | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize );
}

void MenuFramework::EndModule( ) {
	ImGui::EndChild( );
	ImGui::PopStyleVar( );
	ImGui::PopFont( );
	ImGui::PushFont( g_menu.droid_sans_bold );
	ImVec2 p = ImGui::GetWindowPos( );
	ImGui::PopFont( );
	module_position = { 0, 0 };
}

void MenuFramework::BeginModuleHeader( ) {
	ImGui::Columns( 3, NULL, false );
}

void MenuFramework::ContinueModule( ) {
	ImGui::NextColumn( );
}

void MenuFramework::ConcludeModuleHeader( ) {
	ImGui::Columns( );
}

bool MenuFramework::ColorPicker( std::string text, std::string col ) {

	Color tempclr = g_cfg[ col ].get_color( );

	ImGui::SameLine( );
	ImGui::SetCursorPos( ImVec2( ImGui::GetWindowSize( ).x - 50.0f - ImGui::GetStyle( ).FramePadding.x * 2.0f, ImGui::GetCursorPosY( ) + 1 ) );

	float out[ 4 ] = { ( float )tempclr.r( ) / 255.f, ( float )tempclr.g( ) / 255.f , ( float )tempclr.b( ) / 255.f, ( float )tempclr.a( ) / 255.f };
	bool ret = ImGui::ColorEdit4( text.c_str( ), out, ImGuiColorEditFlags_NoInputs );

	if ( ret ) {
		tempclr.r( ) = ( int )( out[ 0 ] * 255.f );
		tempclr.g( ) = ( int )( out[ 1 ] * 255.f );
		tempclr.b( ) = ( int )( out[ 2 ] * 255.f );
		tempclr.a( ) = ( int )( out[ 3 ] * 255.f );
	}

	g_cfg[ col ].set_color( tempclr );

	return ret;
}

void MenuFramework::TextInput( std::string name, char* input, bool long_input ) {
	ImGui::SetCursorPosY( ImGui::GetCursorPosY( ) + 8 );
	ImGui::PushStyleVar( ImGuiStyleVar_FrameRounding, 1.45f );
	ImGui::InputText( false, name.c_str( ), input, 20, 0, 0, 0, long_input );
	ImGui::PopStyleVar( );
	ImGui::SetCursorPosY( ImGui::GetCursorPosY( ) + 10 );
}

void MenuFramework::Button( std::string name, bool* active ) {
	ImGui::PushStyleVar( ImGuiStyleVar_FrameRounding, 1.45f );
	bool enabled = ImGui::Button( name.c_str( ), ImVec2( 177 + 17, 30 ) );
	ImGui::PopStyleVar( );
	*active = enabled;
}

void MenuFramework::Checkbox( std::string name, std::string boolean ) {
	bool b = g_cfg[ ( boolean ) ].get<bool>( );
	ImGui::PushStyleVar( ImGuiStyleVar_FrameRounding, 1.45f );
	ImGui::Checkbox( name.c_str( ), &b );
	ImGui::PopStyleVar( );
	g_cfg[ ( boolean ) ].set<bool>( b );
}

void MenuFramework::KeybindCheckbox( std::string name ) {
	ImGui::PushStyleVar( ImGuiStyleVar_FrameRounding, 1.45f );
	ImGui::ShowCheckbox( name.c_str( ) );
	ImGui::PopStyleVar( );
}

void MenuFramework::NumberPicker( std::string name, std::string value, int min, int max, std::string unit, int step = 1 ) {
	int val = g_cfg[ value ].get<int>( );
	ImGui::PushStyleVar( ImGuiStyleVar_FrameRounding, 1.45f );
	ImGui::SliderInt( name.c_str( ), &val, min, max, unit.c_str( ) );
	ImGui::PopStyleVar( );
	g_cfg[ value ].set<int>( val );
}

void MenuFramework::ComboBox( std::string name, std::string current_item, const char* items, int count ) {
	int cfg_item = g_cfg[ current_item ].get<int>( );

	ImGui::PushStyleVar( ImGuiStyleVar_FrameRounding, 1.45f );
	ImGui::Combo( name.c_str( ), &cfg_item, items, count );
	ImGui::PopStyleVar( );
	g_cfg[ current_item ].set<int>( cfg_item );
}

void MenuFramework::Label( const char* lbl ) {
	ImGui::Text( lbl );
}

void MenuFramework::NumberPicker( std::string name, std::string value, float min, float max, std::string unit, float step = 0.1f ) {
	float val = g_cfg[ value ].get<float>( );
	ImGui::PushStyleVar( ImGuiStyleVar_FrameRounding, 1.45f );
	ImGui::SliderFloat( name.c_str( ), &val, min, max, unit.c_str( ), step );
	ImGui::PopStyleVar( );
	g_cfg[ value ].set<float>( val );
}

bool MenuFramework::GetKey( std::string name, std::string key, std::string keybind_type ) {
	int key_int = g_cfg[ key ].get<int>( );
	int key_int_type = g_cfg[ keybind_type ].get<int>( );
	ImGui::SameLine( );
	ImGui::SetCursorPos( ImVec2( ImGui::GetWindowSize( ).x - ImGui::GetStyle( ).FramePadding.x * 2.0f, ImGui::GetCursorPosY( ) ) );
	ImGui::Hotkey( name.c_str( ), &key_int, ImVec2( 220, 15 ), &key_int_type );
	g_cfg[ key ].set<int>( key_int );
	g_cfg[ keybind_type ].set<int>( key_int_type );
	ImGui::SetCursorPosY( ImGui::GetCursorPosY( ) - 6 );
	return false;
}

bool vector_getter( void* vec_ptr, int index, const char** out_text ) {
	auto& vector = *( std::vector<std::string>* ) vec_ptr;

	if ( index < 0 || index >= int( vector.size( ) ) )
		return false;

	*out_text = vector[ index ].c_str( );
	return true;
}

bool MenuFramework::ListBox( std::string name, std::string current_index, std::vector<std::string> values, int height = -1 ) {
	ImGui::SetCursorPosY( ImGui::GetCursorPosY( ) - 5 );

	int n = g_cfg[ current_index ].get<int>( );

	ImGui::ListBox( name.c_str( ), &n, vector_getter, ( void* )( &values ), values.size( ), height );

	g_cfg[ current_index ].set<int>( n );

	ImGui::SetCursorPosY( ImGui::GetCursorPosY( ) - 10 );
	return true;
}

void MenuFramework::MultiCombo( std::string name, std::vector<std::string> config_values, std::vector<std::string> names ) {

	std::string display_value = "";
	bool is_active = false;
	int amount = 0;
	if ( !names.empty( ) )
		for ( int i = 0; i < names.size( ); i++ ) {
			if ( !g_cfg[ config_values.at( i ) ].get<bool>( ) )
				continue;

			display_value += std::string( amount > 0 ? XOR( ", " ) : XOR( "" ) ) + names.at( i );
			is_active = true;
			amount++;
		}
	if ( !is_active )
		display_value = XOR( "None" );

	if ( ImGui::BeginCombo( name.c_str( ), display_value.c_str( ), ImVec2( 0, ( config_values.size( ) * 22 ) + 5 ) ) ) {
		ImGui::SetCursorPosY( ImGui::GetCursorPosY( ) - 5 );

		for ( int i = 0; i < config_values.size( ); i++ ) {
			bool temp_bool = g_cfg[ config_values.at( i ) ].get<bool>( );
			ImGui::Selectable( ( names.at( i ).c_str( ) ), &temp_bool, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups );
			g_cfg[ config_values.at( i ) ].set<bool>( temp_bool );
		}
		ImGui::EndCombo( );
	}

	ImGui::SetCursorPosY( ImGui::GetCursorPosY( ) + 5 );

}