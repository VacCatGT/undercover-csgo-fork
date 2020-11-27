#include "undercover.h"

void Hooks::DrawModelExecute( uintptr_t ctx, const DrawModelState_t& state, const ModelRenderInfo_t& info, matrix3x4_t* bone ) {
	
	// disable rendering of shadows.
	if ( strstr( info.m_model->m_name, XOR( "shadow" ) ) != nullptr && g_cfg[ XOR( "visuals_misc_remove_entity_shadows" ) ].get<bool>( ) )
		return;

	// disable rendering of sleeves.
	if ( strstr( info.m_model->m_name, XOR( "sleeve" ) ) != nullptr && g_cfg[ XOR( "visuals_misc_remove_sleeves" ) ].get<bool>( ) )
		return;

	// disables csgo's hdr effect.
	static auto hdr = g_csgo.m_material_system->FindMaterial( XOR( "dev/blurfiltery_nohdr" ), XOR( "Other textures" ) );
	hdr->SetFlag( MATERIAL_VAR_NO_DRAW, true );
	
	// dont override glow.
	if ( g_csgo.m_studio_render->IsForcedMaterialOverriden( ) )
		return g_hooks.m_model_render.GetOldMethod< Hooks::DrawModelExecute_t >( IVModelRender::DRAWMODELEXECUTE )( this, ctx, state, info, bone );

	// do chams.
	if( g_chams.DrawModel( ctx, state, info, bone ) ) {
		g_hooks.m_model_render.GetOldMethod< Hooks::DrawModelExecute_t >( IVModelRender::DRAWMODELEXECUTE )( this, ctx, state, info, bone );
		g_csgo.m_studio_render->ForcedMaterialOverride( nullptr );
	}
}