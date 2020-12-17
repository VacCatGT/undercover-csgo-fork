#pragma once


class value {
public:
	template <typename t = float>
	t get() {
		return (t)m_value;
	}

	template <typename t = float>
	void set(t in) { 
		m_value = (double)in;
	}

	Color get_color() {
		int a = ( (D3DCOLOR)m_value >> 24 ) & 0xff;
		int r = ( ( D3DCOLOR ) m_value >> 16 ) & 0xff;
		int g = ( ( D3DCOLOR ) m_value >> 8 ) & 0xff;
		int b = ( D3DCOLOR ) m_value & 0xff;

		if (a == 0 && r == 0 && g == 0 && b == 0) {
			a = 255;
			r = 255;
			g = 255;
			b = 255;
		}

		Color ret( r, g, b, a );
		return ret;
	}

	void set_color(Color in) {
		DWORD color = D3DCOLOR_RGBA(in.r(), in.g(), in.b(), in.a());
		
		m_value = (double)color;
	}


private:
	double m_value = 0.0;
};

extern std::map< std::string, value > g_cfg;

class Config {

public:
	void init( );
	inline static std::vector<std::string> configs{};
	inline static std::unordered_map<std::string, bool> m_hotkey_states{};
	typedef void(*LPSEARCHFUNC)(LPCTSTR lpszFileName);

public:
	bool get_hotkey(std::string str);
	void create( std::string name = XOR( "config.ini" ));
	void save(std::string name = XOR("config.ini"), bool to_clipboard = false);
	void load(std::string name = XOR("config.ini"), bool from_clipboard = false);
	void delet( std::string name = XOR( "config.ini" ) );
	void refresh();
	inline static std::string m_path;

private:
	bool m_init;
};

extern Config g_config;