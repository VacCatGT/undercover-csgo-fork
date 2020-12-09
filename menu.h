#pragma once
class Menu {
public:
	bool m_opened;
	int m_curtab;
public:
	void Think( IDirect3DDevice9* device );
public:
	void __stdcall CreateObjects( IDirect3DDevice9* device ) {
		if ( g_csgo.m_window )
			ImGui_ImplDX9_CreateDeviceObjects( );
	}
public:
	void __stdcall InvalidateObjects( ) {
		ImGui_ImplDX9_InvalidateDeviceObjects();
	}
public:
	void __stdcall SetupPresent( IDirect3DDevice9* device ) {
		ImGui_ImplDX9_Init( g_csgo.m_window, device );

		ImGuiStyle& style = ImGui::GetStyle(  );
		style.Alpha = 1.0f;
		style.WindowPadding = ImVec2( 0, 0 );
		style.WindowMinSize = ImVec2( 32, 32 );
		style.WindowRounding = 0.0f;
		style.WindowTitleAlign = ImVec2( 0.0f, 0.5f );
		style.ChildWindowRounding = 0.0f;
		style.FramePadding = ImVec2( 2, 1 );
		style.FrameRounding = 5.0f;
		style.ItemSpacing = ImVec2( 6, 6 );
		style.ItemInnerSpacing = ImVec2( 8, 8 );
		style.TouchExtraPadding = ImVec2( 0, 0 );
		style.IndentSpacing = 21.0f;
		style.ColumnsMinSpacing = 0.0f;
		style.ScrollbarSize = 6.0f;
		style.ScrollbarRounding = 2.0f;
		style.GrabMinSize = 5.0f;
		style.GrabRounding = 2.0f;
		style.ButtonTextAlign = ImVec2( 0.5f, 0.5f );
		style.DisplayWindowPadding = ImVec2( 22, 22 );
		style.DisplaySafeAreaPadding = ImVec2( 4, 4 );
		style.AntiAliasedLines = true;
		style.AntiAliasedShapes = false;
		style.CurveTessellationTol = 1.f;
	
		ImVec4* colors = ImGui::GetStyle(  ).Colors;
		colors[ ImGuiCol_Text ] = ImVec4( 1.00f, 1.00f, 1.00f, 1.00f );
		colors[ ImGuiCol_TextDisabled ] = ImVec4( 0.50f, 0.50f, 0.50f, 1.00f );
		colors[ ImGuiCol_WindowBg ] = ImVec4( 25 / 255.f, 25 / 255.f, 25 / 255.f, 1.0f );
		colors[ ImGuiCol_PopupBg ] = ImVec4( 0.08f, 0.08f, 0.08f, 0.94f );
		colors[ ImGuiCol_Border ] = ImVec4( 0, 0, 0, 1 );
		colors[ ImGuiCol_BorderShadow ] = ImVec4( 0.00f, 0.00f, 0.00f, 0.00f );
		colors[ ImGuiCol_ChildWindowBg ] = ImVec4( 205/ 255.f, 21 / 255.f, 21 / 255.f, 1 );
		colors[ ImGuiCol_FrameBg ] = ImVec4( 0.22f, 0.22f, 0.22f, 1.0f );
		colors[ ImGuiCol_FrameBgHovered ] = ImVec4( 0.15f, 0.15f, 0.15f, 1.0f );
		colors[ ImGuiCol_FrameBgActive ] = ImVec4( 0.08f, 0.08f, 0.08f, 1.00f );
		colors[ ImGuiCol_TitleBgActive ] = ImVec4( 205 / 255.f, 21 / 255.f, 21 / 255.f, 1.0f );
		colors[ ImGuiCol_TitleBg ] = ImVec4( 205/ 255.f, 21 / 255.f, 21 / 255.f, 1.0f );
		colors[ ImGuiCol_TitleBgCollapsed ] = ImVec4( 205/ 255.f, 21 / 255.f, 21 / 255.f, 1.0f );
		colors[ ImGuiCol_MenuBarBg ] = ImVec4( 0.14f, 0.14f, 0.14f, 1.00f );
		colors[ ImGuiCol_ScrollbarBg ] = ImVec4( 0.17f, 0.17f, 0.17f, 1.00f );
		colors[ ImGuiCol_ScrollbarGrab ] = ImVec4( 0.25f, 0.25f, 0.25f, 1.00f );
		colors[ ImGuiCol_ScrollbarGrabHovered ] = ImVec4( 0.25f, 0.25f, 0.25f, 1.00f );
		colors[ ImGuiCol_ScrollbarGrabActive ] = ImVec4( 0.25f, 0.25f, 0.25f, 1.00f );
		colors[ ImGuiCol_Button ] = ImVec4( 135 / 255.f, 135 / 255.f, 135 / 255.f, 1.0f );
		colors[ ImGuiCol_ButtonHovered ] = ImVec4( 151 / 255.f, 151 / 255.f, 151 / 255.f, 1.0f );
		colors[ ImGuiCol_ButtonActive ] = ImVec4( 135 / 255.f, 135 / 255.f, 135 / 255.f, 1.0f );
		colors[ ImGuiCol_Header ] = ImVec4( 32 / 255.f, 32 / 255.f, 32 / 255.f, 1.0f );
		colors[ ImGuiCol_HeaderHovered ] = ImVec4( 32 / 255.f, 32 / 255.f, 32 / 255.f, 1.0f );
		colors[ ImGuiCol_HeaderActive ] = ImVec4( 32 / 255.f, 32 / 255.f, 32 / 255.f, 1.0f );
		colors[ ImGuiCol_Separator ] = ImVec4( 0, 0, 0, 1 );
		colors[ ImGuiCol_SeparatorHovered ] = ImVec4( 0, 0, 0, 1 );
		colors[ ImGuiCol_SeparatorActive ] = ImVec4( 0, 0, 0, 1 );
		colors[ ImGuiCol_ResizeGrip ] = ImVec4( 0, 0, 0, 0 );
		colors[ ImGuiCol_ResizeGripHovered ] = ImVec4( 0.26f, 0.59f, 0.98f, 0 );
		colors[ ImGuiCol_ResizeGripActive ] = ImVec4( 0.26f, 0.59f, 0.98f, 0 );
		colors[ ImGuiCol_PlotLines ] = ImVec4( 0.61f, 0.61f, 0.61f, 1.00f );
		colors[ ImGuiCol_PlotLinesHovered ] = ImVec4( 1.00f, 0.43f, 0.35f, 1.00f );
		colors[ ImGuiCol_PlotHistogram ] = ImVec4( 0.90f, 0.70f, 0.00f, 1.00f );
		colors[ ImGuiCol_PlotHistogramHovered ] = ImVec4( 1.00f, 0.60f, 0.00f, 1.00f );
		colors[ ImGuiCol_TextSelectedBg ] = ImVec4(0.98f,0.26f, 0.26f, 0.35f );
		colors[ ImGuiCol_CloseButton ] = ImVec4( 0, 0, 0, 0 );
		colors[ ImGuiCol_CloseButtonHovered ] = ImVec4( 0, 0, 0, 0 );
		colors[ ImGuiCol_CheckMark ] = ImVec4( 255 / 255.f, 255 / 255.f, 255 / 255.f, 1.f );
		colors[ ImGuiCol_SliderGrab ] = ImVec4( 255 / 255.f, 255 / 255.f, 255 / 255.f, 1.f );
		colors[ ImGuiCol_SliderGrabActive ] = ImVec4( 255 / 255.f, 255 / 255.f, 255 / 255.f, 1.f );
		colors[ ImGuiCol_Button ] = ImVec4( 0.f, 0.f, 0.f, 0.f );
		colors[ ImGuiCol_ButtonActive ] = ImVec4( 0.f, 0.f, 1.f, 1.f );
		colors[ ImGuiCol_ChildWindowBg ] = ImVec4( 255 / 255.f, 255 / 255.f, 255 / 255.f, 0.f );
		colors[ ImGuiCol_BorderShadow ] = ImVec4(41 / 255.f, 41 / 255.f,41 / 255.f, 1.f );
		colors[ ImGuiCol_WindowBg ] = ImVec4(41 / 255.f, 41 / 255.f,41 / 255.f, 1.f );

		CreateObjects( device );
	}

	void ApplyFonts( ) {
		ImGui::CreateContext( );
		agency = ImGui::GetIO( ).Fonts->AddFontFromMemoryCompressedTTF(josefinSans_compressed_data, josefinSans_compressed_size, 15);
		droid_sans = ImGui::GetIO( ).Fonts->AddFontFromMemoryCompressedTTF( josefinSans_compressed_data, josefinSans_compressed_size, 13);
		droid_sans_bold = ImGui::GetIO( ).Fonts->AddFontFromMemoryCompressedTTF( josefinSans_compressed_data, josefinSans_compressed_size, 13 );
		droid_sans_bold_small = ImGui::GetIO( ).Fonts->AddFontFromMemoryCompressedTTF( josefinSans_compressed_data, josefinSans_compressed_size, 10 );
		segue_ui = ImGui::GetIO( ).Fonts->AddFontFromMemoryCompressedTTF( josefinSans_compressed_data, josefinSans_compressed_size, 14 );
	}

	void SetupTextures( IDirect3DDevice9* device );

	void __stdcall EndPresent( IDirect3DDevice9* device ) {
		ImGui::Render( );
	//	stateBlock->Release( );background_raw_data
	}

	IDirect3DStateBlock9* stateBlock = nullptr;
	IDirect3DVertexDeclaration9* vertDec = nullptr;

	void __stdcall PreRender( IDirect3DDevice9* device ) {
		device->GetVertexDeclaration( &vertDec );
		device->CreateStateBlock( D3DSBT_PIXELSTATE, &stateBlock );
		stateBlock->Capture( );
	}

	void __stdcall PostRender( IDirect3DDevice9* device ) {
		ImGui_ImplDX9_NewFrame( );
//		device->SetVertexDeclaration( vertDec );
	}
public:
	ImFont* agency;
	ImFont* droid_sans_bold;
	ImFont* droid_sans;
	ImFont* segue_ui;
	ImFont* droid_sans_bold_small;
	LPDIRECT3DTEXTURE9 undercover_logo;
	LPDIRECT3DTEXTURE9 undercover_background;
	LPDIRECT3DTEXTURE9 undercover_top_left_line;
	LPDIRECT3DTEXTURE9 undercover_groupbox;
}; extern Menu g_menu;
