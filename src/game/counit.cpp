

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
	m_unit_program(BGFX_INVALID_HANDLE),
    m_state(eUnitState::Run),
    m_shader_param{vec4(0.f), vec4(0.f)},
    m_speed(1.f)
{

}

CoUnit::~CoUnit()
{

}

void CoUnit::Create( Entity* owner, class json::Object* proto )
{
	Super::Create( owner, proto );

	json::TokenIdx ent_tok = proto->GetToken( "entity", json::OBJECT );
	json::TokenIdx unit_tok = proto->GetToken( "unit", json::OBJECT, ent_tok );
	if(unit_tok != INDEX_NONE )
	{
		json::TokenIdx shader_tok = proto->GetToken( "shader", json::STRING, unit_tok);
		if (shader_tok != INDEX_NONE)
		{
			String str_shader("");
			proto->GetStringValue(shader_tok, str_shader);

            String vs = String::Printf("%s_vs", str_shader.c_str());
            String fs = String::Printf("%s_fs", str_shader.c_str());
            m_unit_program = loadProgram(vs.c_str(), fs.c_str());
            m_u_shader_param[0] = bgfx::createUniform("u_custom_0", bgfx::UniformType::Vec4);
            m_u_shader_param[1] = bgfx::createUniform("u_custom_1", bgfx::UniformType::Vec4);
		}

		json::TokenIdx param_tok = proto->GetToken( "speed", json::PRIMITIVE, unit_tok);
        if( param_tok != INDEX_NONE )
			m_speed = proto->GetFloatValue( param_tok, m_speed);        
	}
    
    // create buffers
    {
        // unit quad
        static Draw::QuadVertex unit_vertices[] = { vec2(-0.5f,-0.5f), vec2(0.5f,-0.5f), vec2(0.5f,0.5f), vec2(-0.5f,0.5f) };
        static uint16 idx_data[] = {
            0,2,1, 0,3,2
        };

        m_vbh_quad = bgfx::createVertexBuffer(bgfx::makeRef(unit_vertices, sizeof(unit_vertices)),
            Draw::QuadVertex::ms_layout);

        m_ibh_quad = bgfx::createIndexBuffer(
            // Static data can be passed with bgfx::makeRef
            bgfx::makeRef(idx_data, sizeof(idx_data))
        );
    }
    
}

void CoUnit::Destroy()
{
    bgfx::destroy(m_vbh_quad);
    bgfx::destroy(m_ibh_quad);
    bgfx::destroy(m_u_shader_param[0]);
    bgfx::destroy(m_u_shader_param[1]);
    bgfx::destroy(m_unit_program);

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

void CoUnit::_Render( RenderContext& render_ctxt )
{
	CoPosition* ppos = static_cast<CoPosition*>( GetEntityComponent( CoPosition::StaticClass() ) );
    transform unit_transform = ppos->GetTransform();
	mat4 world_mat( unit_transform.GetRotation(), unit_transform.GetTranslation(), (float)unit_transform.GetScale() );

	// draw unit
	{
        bgfx::setTransform(&world_mat.v0);
        bgfx::setViewTransform(0, &render_ctxt.m_view_mat.v0, &render_ctxt.m_proj_mat.v0);

        bgfx::setUniform(m_u_shader_param[0], &m_shader_param[0]);
        bgfx::setUniform(m_u_shader_param[1], &m_shader_param[1]);

        // Set vertex and index buffer.
        bgfx::setVertexBuffer(0, m_vbh_quad);
        bgfx::setIndexBuffer(m_ibh_quad);

        uint64 state = 0
            | BGFX_STATE_WRITE_RGB
            | BGFX_STATE_WRITE_A
            //| BGFX_STATE_WRITE_Z
            //| BGFX_STATE_DEPTH_TEST_LESS
            | BGFX_STATE_BLEND_ALPHA
            | BGFX_STATE_BLEND_NORMAL
            | BGFX_STATE_CULL_CW
            | BGFX_STATE_MSAA
            ;
        bgfx::setState(state);
        //bgfx::setState(BGFX_STATE_DEFAULT);

        bgfx::submit(0, m_unit_program);
	}
}

vec2 CoUnit::GetScreenPos(mat4 const& view_proj)
{
    CoPosition* ppos = static_cast<CoPosition*>( GetEntityComponent( CoPosition::StaticClass() ) );
    transform unit_transform = ppos->GetTransform();
    vec4 proj_pos = view_proj * vec4( unit_transform.GetTranslation(), 1.f );
    vec2 pos = vec2( proj_pos.x / proj_pos.w, proj_pos.y / proj_pos.w );
    return pos;
}

void CoUnit::ChangeState( eUnitState new_state )
{
    m_state = new_state;
}

void CoUnit::SetCurrentLevel( Entity* current_level )
{
    m_current_level = current_level;
}
