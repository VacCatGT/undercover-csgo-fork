#include "tapped.h"
#define PHYS_PYSHSCALE_NORMAL 1

void Hooks::PaintTraverse( VPANEL panel, bool repaint, bool force ) {
	static VPANEL tools{ }, zoom{ };

	// don't call the original function if we want to remove the scope.
	if ( panel && g_cfg[ XOR( "visuals_misc_remove_scope" ) ].get< bool >( ) && !strcmp( XOR( "HudZoom" ), g_csgo.m_panel->GetName( panel ) ) )
		return;

	g_hooks.m_panel.GetOldMethod< PaintTraverse_t >( IPanel::PAINTTRAVERSE )( this, panel, repaint, force );

	if ( !panel && !strcmp( XOR( "MatSystemTopPanel" ), g_csgo.m_panel->GetName( panel ) ) ) panel = panel;
	else if ( panel != panel )
		return;

	static uint32_t focus_overlay_panel{ };
	if ( !focus_overlay_panel ) {

		std::string panelname( g_csgo.m_panel->GetName( panel ) );

		if ( panelname.find( XOR( "FocusOverlayPanel" ) ) != std::string::npos )
			focus_overlay_panel = panel;
	}

	// render hack stuff.
	if ( panel == focus_overlay_panel ) {
		static bool flip = false;
		flip = !flip;
		if ( !flip ) return;

		g_cl.OnPaint( );
	}
}