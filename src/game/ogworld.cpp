

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
    json::TokenIdx ltok = jsn_obj.GetToken( "levels", json::ARRAY, wtok );
    int level_count = jsn_obj.GetArraySize( ltok );
    
    for( int level_idx = 0; level_idx < level_count; level_idx++ )
    {
        String level_name;
        jsn_obj.GetArrayStringValue( ltok, level_idx, level_name );
        
        String level_path = String::Printf("../data/level/%s/", level_name.c_str());
        String level_json = level_path + "level.json";
        Entity* plevel = EntityManager::GetStaticInstance()->CreateEntityFromJson( level_json.c_str(), level_name.c_str() );
        if( plevel )
            EntityManager::GetStaticInstance()->AddEntityToWorld( plevel );
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

