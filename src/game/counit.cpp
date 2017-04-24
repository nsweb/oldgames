

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
	m_shader(nullptr),
    m_state(eUnitState::Run),
    m_shader_param{vec4(0.f)},
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
			m_shader = GfxManager::GetStaticInstance()->LoadShader(str_shader);
		}

		json::TokenIdx param_tok = proto->GetToken( "speed", json::PRIMITIVE, unit_tok);
        if( param_tok != INDEX_NONE )
			m_speed = proto->GetFloatValue( param_tok, m_speed);        
	}
    
    if(m_shader)
    {
        glGenVertexArrays( eVACount, m_varrays );
        glGenBuffers( eVBCount, m_vbuffers );
        
        // unit quad
        const vec2 unit_vertices[] = { vec2(-0.5f,-0.5f), vec2(0.5f,-0.5f), vec2(0.5f,0.5f), vec2(-0.5f,0.5f) };
        GLuint idx_data[] = {
            0,2,1, 0,3,2
        };
        
        glBindVertexArray( m_varrays[eVAUnit] );
        {
            glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_vbuffers[eVBUnitElt] );
            glBufferData( GL_ELEMENT_ARRAY_BUFFER, COUNT_OF(idx_data) * sizeof(GLuint), idx_data, GL_STATIC_DRAW );
            
            glBindBuffer( GL_ARRAY_BUFFER, m_vbuffers[eVBUnit] );
            glBufferData( GL_ARRAY_BUFFER, COUNT_OF(unit_vertices) * sizeof(vec2), unit_vertices, GL_STATIC_DRAW );
            glEnableVertexAttribArray(0);
            glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, sizeof(vec2) /*stride*/, (void*)0 /*offset*/ );
            
            glBindVertexArray(0);
            for( int attrib_idx = 0; attrib_idx < 8; attrib_idx++)
                glDisableVertexAttribArray( attrib_idx);
        }
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

void CoUnit::_Render( RenderContext& render_ctxt )
{
    if(!m_shader)
        return;

	transform cam2world_transform( render_ctxt.m_view.m_transform.GetRotation(), render_ctxt.m_view.m_transform.GetTranslation(), (float)render_ctxt.m_view.m_transform.GetScale() );
	mat4 view_inv_mat( cam2world_transform.GetRotation(), cam2world_transform.GetTranslation(), cam2world_transform.GetScale() );

	CoPosition* ppos = static_cast<CoPosition*>( GetEntityComponent( CoPosition::StaticClass() ) );
    transform unit_transform = ppos->GetTransform();
	mat4 world_mat( unit_transform.GetRotation(), unit_transform.GetTranslation(), (float)unit_transform.GetScale() );
	mat4 view_mat = bigball::inverse(view_inv_mat);

	m_shader->Bind();
	{
		ShaderUniform uni_world = m_shader->GetUniformLocation("world_mat");
		m_shader->SetUniform( uni_world, world_mat );
		ShaderUniform uni_view = m_shader->GetUniformLocation("view_mat");
		m_shader->SetUniform( uni_view, view_mat );
		ShaderUniform uni_proj = m_shader->GetUniformLocation("proj_mat");
		m_shader->SetUniform( uni_proj, render_ctxt.m_proj_mat );
        ShaderUniform uni_time = m_shader->GetUniformLocation("custom_0");
        m_shader->SetUniform( uni_time, m_shader_param[0] );
        ShaderUniform uni_alpha = m_shader->GetUniformLocation("custom_1");
        m_shader->SetUniform( uni_alpha, m_shader_param[1] );
		
        glBindVertexArray( m_varrays[eVAUnit] );
        
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        
        glBindVertexArray(0);
	}
	m_shader->Unbind();

}

void CoUnit::ChangeState( eUnitState new_state )
{
    m_state = new_state;
}

void CoUnit::SetCurrentLevel( Entity* current_level )
{
    m_current_level = current_level;
}
