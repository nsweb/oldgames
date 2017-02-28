

#include "../oldgames.h"
#include "copmunit.h"
#include "copmlevel.h"

//#include "core/json.h"
//#include "system/file.h"
#include "engine/coposition.h"
#include "engine/controller.h"
//#include "engine/camera.h"
//#include "engine/tickcontext.h"
//#include "engine/entitymanager.h"
//#include "gfx/gfxmanager.h"
//#include "gfx/shader.h"
//#include "gfx/rendercontext.h"
//#include "gfx/drawutils.h"

CLASS_EQUIP_CPP(CoPmUnit);

CoPmUnit::CoPmUnit() :
	m_move_vector(0.f,0.f)
{

}

CoPmUnit::~CoPmUnit()
{

}

void CoPmUnit::Create( Entity* owner, class json::Object* proto )
{
	Super::Create( owner, proto );
}

void CoPmUnit::Destroy()
{
	Super::Destroy();
}

void CoPmUnit::AddToWorld()
{
	Super::AddToWorld();
}

void CoPmUnit::RemoveFromWorld()
{
	Super::RemoveFromWorld();
}

void CoPmUnit::Tick( TickContext& tick_ctxt )
{
	Super::Tick(tick_ctxt);
}

void CoPmUnit::OnControllerInput( ControllerInput const& input, CoPmLevel* level)
{
	CoPosition* copos = static_cast<CoPosition*>(GetEntityComponent("CoPosition"));
	if (copos)
	{
		vec3 unit_pos = copos->GetPosition();
		PmTile& tile = level->GetTile(unit_pos.xy);

		vec3 right(1.0f, 0.0f, 0.0f);
		vec3 down(0.0f, 1.0f, 0.0f);
		unit_pos += (right * input.m_delta.x + down * -input.m_delta.y) * m_speed;
		copos->SetPosition(unit_pos);
	}
}
