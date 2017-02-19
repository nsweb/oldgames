

#include "../oldgames.h"
#include "colevel.h"
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

CLASS_EQUIP_CPP(CoLevel);

CoLevel::CoLevel()
{

}

CoLevel::~CoLevel()
{

}

void CoLevel::Create( Entity* owner, class json::Object* proto )
{
	Super::Create( owner, proto );
    
    m_level_name = GetEntity()->GetName();

	/*json::TokenIdx EntTok = proto->GetToken( "entity", json::OBJECT );
	json::TokenIdx ShipTok = proto->GetToken( "Ship", json::OBJECT, EntTok );
	if( ShipTok != INDEX_NONE )
	{
		json::TokenIdx ParamTok = proto->GetToken( "speed", json::PRIMITIVE, ShipTok );
		if( ParamTok != INDEX_NONE )
			m_speed = proto->GetFloatValue( ParamTok, m_speed );
	}*/
    
}

void CoLevel::Destroy()
{
 
    
	Super::Destroy();
}

void CoLevel::AddToWorld()
{
    
	Super::AddToWorld();
}

void CoLevel::RemoveFromWorld()
{
	Super::RemoveFromWorld();
}

void CoLevel::Tick( TickContext& tick_ctxt )
{

}

void CoLevel::_Render( RenderContext& render_ctxt)
{
    
}



