

#include "../oldgames.h"
#include "counit.h"

#include "core/json.h"
#include "system/file.h"
#include "engine/coposition.h"
#include "engine/controller.h"
#include "engine/camera.h"
#include "engine/tickcontext.h"
#include "engine/entitymanager.h"
#include "gfx/gfxmanager.h"
#include "gfx/shader.h"
#include "gfx/rendercontext.h"
#include "gfx/drawutils.h"

CLASS_EQUIP_CPP(CoUnit);

CoUnit::CoUnit() :
	m_current_level(nullptr),
    m_state(eUnitState::Run),
    m_speed(1.f),
    m_speed_lateral(0.1f),
    m_cam_zoffset(0.05f),
    m_unit_scale(0.005f),
    m_move_range(0.03f)
{

}

CoUnit::~CoUnit()
{

}

void CoUnit::Create( Entity* owner, class json::Object* proto )
{
	Super::Create( owner, proto );

	json::TokenIdx ent_tok = proto->GetToken( "entity", json::OBJECT );
	json::TokenIdx ship_tok = proto->GetToken( "Ship", json::OBJECT, ent_tok );
	if( ship_tok != INDEX_NONE )
	{
		json::TokenIdx param_tok = proto->GetToken( "speed", json::PRIMITIVE, ship_tok );
		if( param_tok != INDEX_NONE )
			m_speed = proto->GetFloatValue( param_tok, m_speed );
        param_tok = proto->GetToken( "cam_zoffset", json::PRIMITIVE, ship_tok );
        if( param_tok != INDEX_NONE )
            m_cam_zoffset = proto->GetFloatValue( param_tok, m_cam_zoffset );
        param_tok = proto->GetToken( "ship_scale", json::PRIMITIVE, ship_tok );
        if( param_tok != INDEX_NONE )
            m_unit_scale = proto->GetFloatValue( param_tok, m_unit_scale );
        param_tok = proto->GetToken( "move_range", json::PRIMITIVE, ship_tok );
        if( param_tok != INDEX_NONE )
            m_move_range = proto->GetFloatValue( param_tok, m_move_range );
	}
    
}

void CoUnit::Destroy()
{


	Super::Destroy();
}

void CoUnit::AddToWorld()
{
	Super::AddToWorld();
}

void CoUnit::RemoveFromWorld()
{
	Super::RemoveFromWorld();
}

void CoUnit::Tick( TickContext& tick_ctxt )
{
	if( !m_current_level )
		return;
}

bool CoUnit::OnControllerInput( Camera* camera, ControllerInput const& input )
{

    return true;
}

void CoUnit::_Render( RenderContext& render_ctxt )
{
#if 0
	static float global_time = 0.f;
	global_time += render_ctxt.m_delta_seconds;

	transform cam2world_transform( render_ctxt.m_view.m_transform.GetRotation(), render_ctxt.m_view.m_transform.GetTranslation(), (float)render_ctxt.m_view.m_transform.GetScale() );

	//vec3 cam_pos = cam2world_transform.GetTranslation();
	mat4 view_inv_mat( cam2world_transform.GetRotation(), cam2world_transform.GetTranslation(), cam2world_transform.GetScale() );

	CoPosition* ppos = static_cast<CoPosition*>( GetEntityComponent( CoPosition::StaticClass() ) );
	transform ship2cam_transform = ppos->GetTransform();	// relative to cam
	transform ship_transform = cam2world_transform * ship2cam_transform;
	mat4 world_mat( ship_transform.GetRotation(), ship_transform.GetTranslation(), (float)ship_transform.GetScale() * m_ship_scale );
	mat4 view_mat = bigball::inverse(view_inv_mat);
	mat4 world_view_mat = view_mat * world_mat;
	static float coll_threshold = 0.05f;
	float collision_dist = clamp(m_current_collision_dist / coll_threshold, 0.0f, 1.0f);

	// Draw reverse faces, so that we can still display something inside cube
	glCullFace(GL_FRONT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	m_shader->Bind();
	{
		ShaderUniform uni_world = m_shader->GetUniformLocation("world_mat");
		m_shader->SetUniform( uni_world, world_mat );
		ShaderUniform uni_view = m_shader->GetUniformLocation("view_mat");
		m_shader->SetUniform( uni_view, view_mat );
		ShaderUniform uni_proj = m_shader->GetUniformLocation("proj_mat");
		m_shader->SetUniform( uni_proj, render_ctxt.m_proj_mat );
		ShaderUniform uni_vtb = m_shader->GetUniformLocation("viewtobox_mat");
		m_shader->SetUniform( uni_vtb, bigball::inverse(world_view_mat) );
		ShaderUniform uni_cdist = m_shader->GetUniformLocation("collision_dist");
		m_shader->SetUniform(uni_cdist, collision_dist);
		
		DFManager::GetStaticInstance()->DrawCube();
	}
	m_shader->Unbind();

	glCullFace(GL_BACK);
#endif // 0
}

void CoUnit::ChangeState( eUnitState new_state )
{
    m_state = new_state;
}

void CoUnit::SetCurrentLevel( Entity* current_level )
{
    m_current_level = current_level;
    
}
