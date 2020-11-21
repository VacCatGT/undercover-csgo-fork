#pragma once

enum tabs {
	tab_one,
	tab_two,
	tab_three,
	tab_four
};

enum sizes {
	full = 1,
	half
};

enum positions {
	left_top = 1,
	left_bottom,
	center_top,
	center_bottom,
	right_top,
	right_bottom
};

class MenuFramework {
private:
	ImVec2 module_position;
	std::string module_name;

public:
	int					 RenderSecondaryHeader( int& subtab, const std::vector<std::string> subtab_names, float padding = 80.f );
	void				 DrawBackdrop( );
	int					 RenderTabBar( int& page, const std::vector<const char*> tabs );
	void				 StartWindow( );
	void				 EndWindow( );
	void				 RenderModule( positions position, sizes size, std::string title );
	void				 EndModule( );
	void				 BeginModuleHeader( );
	void				 ContinueModule( );
	bool				 ColorPicker( std::string text, std::string col );
	void				 ConcludeModuleHeader( );
	void				 TextInput( std::string name, char* input, bool long_input );
	void				 Button( std::string name, bool* active );
	void				 Checkbox( std::string name, std::string boolean );
	void				 KeybindCheckbox( std::string name );
	void				 NumberPicker( std::string name, std::string value, int min, int max, std::string unit, int step );
	bool				 GetKey( std::string name, std::string key, std::string keybind_type );
	bool				 ListBox( std::string name, std::string current_index, std::vector<std::string> values, int height );
	void				 MultiCombo( std::string name, std::vector<std::string> config_values, std::vector<std::string> names );
	void				 ComboBox( std::string name, std::string current_item, const char* items, int count );
	void				 Label( const char* lbl );
	void				 NumberPicker( std::string name, std::string value, float min, float max, std::string unit, float step );

}; extern MenuFramework g_frw;
