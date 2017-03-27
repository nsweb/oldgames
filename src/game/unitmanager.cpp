

#include "../oldgames.h"
#include "unitmanager.h"
#include "counit.h"
#include "engine/controller.h"
#include "engine/camera.h"
#include "gfx/gfxmanager.h"
#include "gfx/shader.h"
#include "gfx/rendercontext.h"
#include "system/profiler.h"


STATIC_MANAGER_CPP(UnitManager);

UnitManager::UnitManager()
{
	m_pStaticInstance = this;
}

UnitManager::~UnitManager()
{
	m_pStaticInstance = nullptr;
}

void UnitManager::Create()
{
	//m_BlockShader = GfxManager::GetStaticInstance()->LoadShader( "block" );
}

void UnitManager::Destroy()
{

}


void UnitManager::AddComponentToWorld( Component* component )
{
	if( component->IsA( CoUnit::StaticClass() ) )
	{
		m_units.push_back( reinterpret_cast<CoUnit*>( component ) );
	}
}

void UnitManager::RemoveComponentFromWorld( Component* component )
{
	if( component->IsA( CoUnit::StaticClass() ) )
	{
		m_units.remove( reinterpret_cast<CoUnit*>( component ) );
	}
}

void UnitManager::Tick( TickContext& tick_ctxt )
{
	PROFILE_SCOPE( __FUNCTION__ );

	for( int32 i = 0; i < m_units.size(); ++i )
	{
		m_units[i]->Tick( tick_ctxt );
	}
}

void UnitManager::_Render( RenderContext& render_ctxt )
{
	PROFILE_SCOPE( __FUNCTION__ );
    
    //glEnable(GL_BLEND);
    //glBlendEquation(GL_FUNC_ADD);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);

	for( int32 i = 0; i < m_units.size(); ++i )
	{
		m_units[i]->_Render( render_ctxt );
	}
}

