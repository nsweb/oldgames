

#include "../oldgames.h"
#include "ogworld.h"
#include "colevel.h"
#include "core/json.h"
#include "engine/controller.h"
#include "engine/camera.h"
#include "engine/entity.h"
#include "engine/entitymanager.h"
#include "gfx/gfxmanager.h"
#include "gfx/shader.h"
#include "gfx/rendercontext.h"
#include "system/profiler.h"


STATIC_MANAGER_CPP(OGWorld);

OGWorld::OGWorld() :
    m_current_level_idx(INDEX_NONE)
	//m_levelshader(nullptr)
{
	m_pStaticInstance = this;
}

OGWorld::~OGWorld()
{
	m_pStaticInstance = nullptr;
}

void OGWorld::Create()
{
	//m_levelshader = new LevelShader();
}

void OGWorld::Destroy()
{
	//BB_DELETE(m_levelshader);
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
}

void OGWorld::_Render( RenderContext& render_ctxt )
{
	PROFILE_SCOPE( __FUNCTION__ );

    for( int32 i = 0; i < m_levels.size(); ++i )
    {
        m_levels[i]->_Render( render_ctxt );
    }
}

