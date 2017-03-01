

#include "../oldgames.h"
#include "copmunit.h"
#include "copmlevel.h"

//#include "core/json.h"
//#include "system/file.h"
#include "engine/coposition.h"
#include "engine/controller.h"
//#include "engine/camera.h"
#include "engine/tickcontext.h"
//#include "engine/entitymanager.h"
//#include "gfx/gfxmanager.h"
//#include "gfx/shader.h"
//#include "gfx/rendercontext.h"
//#include "gfx/drawutils.h"

CLASS_EQUIP_CPP(CoPmUnit);

CoPmUnit::CoPmUnit() :
    m_input_vector(0.f,0.f),
	m_move_vector(0.f,0.f),
    m_current_level(nullptr)
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

void CoPmUnit::BeginPlay( CoPmLevel* level )
{
    m_current_level = level;
    
    // reset unit pos
    CoPosition* copos = static_cast<CoPosition*>(GetEntityComponent("CoPosition"));
    if (copos)
    {
        // top left corner
        copos->SetPosition(vec3(0, 0, 0));
    }
}

void CoPmUnit::Tick( TickContext& tick_ctxt )
{
	Super::Tick(tick_ctxt);
    
    CoPosition* copos = static_cast<CoPosition*>(GetEntityComponent("CoPosition"));
    if (copos)
    {
        float dmove = m_speed * tick_ctxt.m_delta_seconds;
        const vec3 unit_offset(0.5f, 0.5f, 0.0f);
        vec3 unit_pos = copos->GetPosition() + unit_offset;
        vec2 fxy;
        PmTile& tile = m_current_level->GetTile(unit_pos.xy, fxy);
        
        // check if we can consume the input
        if (m_input_vector.x < 0.f)
        {
            if ((fxy.y >= 0.5f - dmove && fxy.y <= 0.5f + dmove) && (fxy.x > 0.5f || tile.m_left))
            {
                //unit_pos.y = 0.5f;
                m_move_vector = vec2(-dmove, 0.f);
                m_input_vector = vec2(0.f, 0.f);
            }
        }
        else if (m_input_vector.x > 0.f)
        {
            if ((fxy.y >= 0.5f - dmove && fxy.y <= 0.5f + dmove) && (fxy.x < 0.5f || tile.m_right))
            {
                //unit_pos.y = 0.5f;
                m_move_vector = vec2(dmove, 0.f);
                m_input_vector = vec2(0.f, 0.f);
            }
        }
        else if (m_input_vector.y < 0.f)
        {
            if ((fxy.x >= 0.5f - dmove && fxy.x <= 0.5f + dmove) && (fxy.y < 0.5f || tile.m_down))
            {
                //unit_pos.x = 0.5f;
                m_move_vector = vec2(0.f, -dmove);
                m_input_vector = vec2(0.f, 0.f);
            }
        }
        else if (m_input_vector.y > 0.f)
        {
            if ((fxy.x >= 0.5f - dmove && fxy.x <= 0.5f + dmove) && (fxy.y > 0.5f || tile.m_up))
            {
                //unit_pos.x = 0.5f;
                m_move_vector = vec2(0.f, dmove);
                m_input_vector = vec2(0.f, 0.f);
            }
        }

        unit_pos.x += m_move_vector.x;
        unit_pos.y -= m_move_vector.y;
        copos->SetPosition(unit_pos - unit_offset);
    }
}

void CoPmUnit::OnControllerInput( ControllerInput const& input)
{
    if (input.m_delta.x > 0.f)
        m_input_vector = vec2(1.f, 0.f);
    else if (input.m_delta.x < 0.f)
        m_input_vector = vec2(-1.f, 0.f);
    if (input.m_delta.y > 0.f)
        m_input_vector = vec2(0.f, 1.f);
    else if (input.m_delta.y < 0.f)
        m_input_vector = vec2(0.f, -1.f);
}
