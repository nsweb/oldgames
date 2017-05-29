

#include "../oldgames.h"
#include "ogworld.h"
#include "colevel.h"
#include "counit.h"
#include "core/json.h"
#include "engine/controller.h"
#include "engine/camera.h"
#include "engine/entity.h"
#include "engine/entitymanager.h"
#include "engine/tickcontext.h"
#include "gfx/gfxmanager.h"
#include "gfx/shader.h"
#include "gfx/rendercontext.h"
#include "system/profiler.h"


STATIC_MANAGER_CPP(OGWorld);

OGWorld::OGWorld() :
    m_current_level_idx(INDEX_NONE),
    m_game_state{0},
    m_varrays{0},
    m_vbuffers{0},
    m_transition_shader(nullptr)
{
	m_pStaticInstance = this;
}

OGWorld::~OGWorld()
{
	m_pStaticInstance = nullptr;
}

void OGWorld::Create()
{
    LoadShaders();
    CreateBuffers();
}

void OGWorld::Destroy()
{
    DestroyBuffers();
}

bool OGWorld::InitLevels( char const* json_path )
{
    json::Object jsn_obj;
    if( !jsn_obj.ParseFile( json_path ) )
    {
        BB_LOG( OGWorld, Log, "OGWorld::InitLevels : cannot open file <%s>", json_path );
        return false;
    }
    
    json::TokenIdx wtok = jsn_obj.GetToken( "world", json::OBJECT );
    const char* game_names[] = { "pacman"};
    for( int game_idx = 0; game_idx < COUNT_OF(game_names); game_idx++ )
    {
        json::TokenIdx gtok = jsn_obj.GetToken( game_names[game_idx], json::OBJECT, wtok );
        
        json::TokenIdx htok = jsn_obj.GetToken( "hero", json::STRING, gtok );
        Entity* hero = nullptr;
        String hero_name = "";
        if (jsn_obj.GetStringValue(htok, hero_name))
        {
            String hero_json = String::Printf("../data/%s/%s.json", game_names[game_idx], hero_name.c_str());
            hero = EntityManager::GetStaticInstance()->CreateEntityFromJson( hero_json.c_str(), hero_name.c_str());
            if (hero)
                EntityManager::GetStaticInstance()->AddEntityToWorld(hero);
        }
        
        json::TokenIdx ltok = jsn_obj.GetToken( "levels", json::ARRAY, gtok );
        int level_count = jsn_obj.GetArraySize( ltok );
        
        for( int level_idx = 0; level_idx < level_count; level_idx++ )
        {
            String level_name;
            jsn_obj.GetArrayStringValue( ltok, level_idx, level_name );
            
            String level_json = String::Printf("../data/%s/%s.json", game_names[game_idx], level_name.c_str());
            Entity* level = EntityManager::GetStaticInstance()->CreateEntityFromJson( level_json.c_str(), level_name.c_str() );
            if( level )
            {
                EntityManager::GetStaticInstance()->AddEntityToWorld( level );
                CoLevel* colevel = static_cast<CoLevel*>( level->GetCompatibleComponent("CoLevel") );
                if (colevel)
                    colevel->SetHero(hero);
            }
        }
    }
    
    if( m_levels.size() )
        m_current_level_idx = 0;
    
    return true;
}

CoLevel* OGWorld::GetCurrentLevel()
{
    if( m_current_level_idx >= 0 )
        return m_levels[m_current_level_idx];
    
    return nullptr;
}

bool OGWorld::SetCurrentLevel( int level_idx )
{
    if( level_idx >= 0 && level_idx < m_levels.size() )
    {
        m_current_level_idx = level_idx;
        return true;
    }
    return false;
}

void OGWorld::AddComponentToWorld( Component* component )
{
	if( component->IsA( CoLevel::StaticClass() ) )
	{
		m_levels.push_back( reinterpret_cast<CoLevel*>( component ) );
	}
}

void OGWorld::RemoveComponentFromWorld( Component* component )
{
	if( component->IsA( CoLevel::StaticClass() ) )
	{
		m_levels.remove( reinterpret_cast<CoLevel*>( component ) );
	}
}

void OGWorld::Tick( TickContext& tick_ctxt )
{
	PROFILE_SCOPE( __FUNCTION__ );

	for( int32 i = 0; i < m_levels.size(); ++i )
	{
		m_levels[i]->Tick( tick_ctxt );
	}
    
    if (m_game_state.m_transition_time > 0.f)
    {
        m_game_state.m_transition_time -= tick_ctxt.m_delta_seconds;
        if (m_game_state.m_transition_time <= 0.f)
        {
            m_game_state.m_transition_time = 0.f;
            
            switch (m_game_state.m_transition_type) {
                case eTransitionType::PlayerDies:
                {
                    m_game_state.m_player_life--;
                    //if (m_game_state.m_player_life > 0)
                        GetCurrentLevel()->BeginPlay(false);
                }
                break;
                
                default:
                break;
            }
            m_game_state.m_transition_type = eTransitionType::None;
        }
    }
}

void OGWorld::_Render( RenderContext& render_ctxt )
{
	PROFILE_SCOPE( __FUNCTION__ );

    for( int32 i = 0; i < m_levels.size(); ++i )
    {
        m_levels[i]->_Render( render_ctxt );
    }
    
    // render transition
    //m_game_state.m_transition_time = 0.5f;
    if (m_game_state.m_transition_time > 0.f )
    {
        CoLevel* level = GetCurrentLevel();
        CoUnit* unit = static_cast<CoUnit*>( level ? level->m_hero->GetCompatibleComponent("CoUnit") : nullptr );
        vec2 unit_pos = unit->GetScreenPos(render_ctxt.m_proj_mat * render_ctxt.m_view_mat);
        
        Shader* shader = m_transition_shader;
        shader->Bind();
        {
            ShaderUniform uni_type = shader->GetUniformLocation("transition_type");
            shader->SetUniform( uni_type, unit_pos );
            ShaderUniform uni_pos = shader->GetUniformLocation("transition_pos");
            shader->SetUniform( uni_pos, unit_pos );
            ShaderUniform uni_time = shader->GetUniformLocation("transition_time");
            shader->SetUniform( uni_time, m_game_state.m_transition_time );
            
            glBindVertexArray( m_varrays[eVATransition] );

            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
        }
        shader->Unbind();
    }
}

void OGWorld::RequestTransition( eTransitionType type )
{
    m_game_state.m_transition_type = type;
    m_game_state.m_transition_time = 4.f;
}

void OGWorld::LoadShaders()
{
    m_transition_shader = GfxManager::GetStaticInstance()->LoadShader( "transition" );
}

void OGWorld::CreateBuffers()
{
    glGenVertexArrays( eVACount, m_varrays );
    glGenBuffers( eVBCount, m_vbuffers );
    
    
    //////////////////////////////////////////////////////////
    // transition quad
    const vec2 tile_vertices[] = { vec2(-1.f,-1.f), vec2(1.f,-1.f), vec2(1.f,1.f), vec2(-1.f,1.f) };
    
    GLuint idx_data[] = {
        0,1,2, 0,2,3
    };
    
    glBindVertexArray( m_varrays[eVATransition] );
    {
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_vbuffers[eVBTransitionElt] );
        glBufferData( GL_ELEMENT_ARRAY_BUFFER, COUNT_OF(idx_data) * sizeof(GLuint), idx_data, GL_STATIC_DRAW );
        
        glBindBuffer( GL_ARRAY_BUFFER, m_vbuffers[eVBTransition] );
        glBufferData( GL_ARRAY_BUFFER, COUNT_OF(tile_vertices) * sizeof(vec2), tile_vertices, GL_STATIC_DRAW );
        glEnableVertexAttribArray(0);
        glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, sizeof(vec2) /*stride*/, (void*)0 /*offset*/ );
        
        glBindVertexArray(0);
        for( int attrib_idx = 0; attrib_idx < 8; attrib_idx++)
            glDisableVertexAttribArray( attrib_idx);
    }
    
}

void OGWorld::DestroyBuffers()
{
    glDeleteBuffers( eVBCount, m_vbuffers );
    glDeleteVertexArrays( eVACount, m_varrays );
}

