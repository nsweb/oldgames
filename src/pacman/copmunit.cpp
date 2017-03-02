

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
    m_current_level(nullptr),
    m_hero(false)
{

}

CoPmUnit::~CoPmUnit()
{

}

void CoPmUnit::Create( Entity* owner, class json::Object* proto )
{
	Super::Create( owner, proto );
    
    // temp
    m_hero = true;
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
        copos->SetPosition(vec3(0.5f, 0.5f, 0));
    }
}

void CoPmUnit::Tick( TickContext& tick_ctxt )
{
	Super::Tick(tick_ctxt);
    
    CoPosition* copos = static_cast<CoPosition*>(GetEntityComponent("CoPosition"));
    if (copos)
    {
        float dmove = m_speed * tick_ctxt.m_delta_seconds;
        vec3 unit_pos = copos->GetPosition();
        vec2 fxy;
        ivec2 tile_coord = m_current_level->GetTileCoord(unit_pos.xy, fxy);
        PmTile& tile = m_current_level->GetTile(tile_coord.x, tile_coord.y);
        PmTileBall& tile_ball = m_current_level->GetTileBall(tile_coord.x, tile_coord.y);
        
        // check if we eat some balls
        if (m_hero)
        {
            const float eat_margin = 0.25f;
            int ball_eaten = 0;
            if (tile_ball.m_right && fxy.x > 1.f - eat_margin)
            {
                tile_ball.m_right = 0;
                ball_eaten++;
            }
            if (tile_ball.m_down && fxy.y > 1.f - eat_margin)
            {
                tile_ball.m_down = 0;
                ball_eaten++;
            }
            if (tile_ball.m_center &&   fxy.x > 0.5f - eat_margin && fxy.x < 0.5f + eat_margin &&
                                        fxy.y > 0.5f - eat_margin && fxy.y < 0.5f + eat_margin)
            {
                tile_ball.m_center = 0;
                ball_eaten++;
            }
            
            if (ball_eaten > 0)
                m_current_level->NeedBallRedraw();
            
        }
        
        // check if we can consume any input
        if (m_input_vector.x < 0.f)
        {
            if ((fxy.y >= 0.5f - dmove && fxy.y <= 0.5f + dmove) && (fxy.x > 0.5f || tile.m_left))
            {
                unit_pos.y -= fxy.y - 0.5f;
                m_move_vector = vec2(-dmove, 0.f);
                m_input_vector = vec2::zero;
            }
        }
        else if (m_input_vector.x > 0.f)
        {
            if ((fxy.y >= 0.5f - dmove && fxy.y <= 0.5f + dmove) && (fxy.x < 0.5f || tile.m_right))
            {
                unit_pos.y -= fxy.y - 0.5f;
                m_move_vector = vec2(dmove, 0.f);
                m_input_vector = vec2::zero;
            }
        }
        else if (m_input_vector.y < 0.f)
        {
            if ((fxy.x >= 0.5f - dmove && fxy.x <= 0.5f + dmove) && (fxy.y < 0.5f || tile.m_down))
            {
                unit_pos.x -= fxy.x - 0.5f;
                m_move_vector = vec2(0.f, -dmove);
                m_input_vector = vec2::zero;
            }
        }
        else if (m_input_vector.y > 0.f)
        {
            if ((fxy.x >= 0.5f - dmove && fxy.x <= 0.5f + dmove) && (fxy.y > 0.5f || tile.m_up))
            {
                unit_pos.x -= fxy.x - 0.5f;
                m_move_vector = vec2(0.f, dmove);
                m_input_vector = vec2::zero;
            }
        }

        unit_pos.x += m_move_vector.x;
        unit_pos.y -= m_move_vector.y;
        
        // check walls
        float dx = bigball::fract(unit_pos.x) - 0.5f;
        float dy = bigball::fract(unit_pos.y) - 0.5f;
        if (m_move_vector.x < 0.f && !tile.m_left && dx <= 0.0f)
        {
            unit_pos.x -= dx;
            m_move_vector = vec2::zero;
        }
        else if (m_move_vector.x > 0.f && !tile.m_right && dx >= 0.0f)
        {
            unit_pos.x -= dx;
            m_move_vector = vec2::zero;
        }
        else if (m_move_vector.y < 0.f && !tile.m_down && dy >= 0.0f)
        {
            unit_pos.y -= dy;
            m_move_vector = vec2::zero;
        }
        else if (m_move_vector.y > 0.f && !tile.m_up && dy <= 0.0f)
        {
            unit_pos.y -= dy;
            m_move_vector = vec2::zero;
        }
        
        copos->SetPosition(unit_pos);
        if (m_move_vector.x > 0.f)
            copos->SetRotation(quat::fromeuler_xyz(vec3(0, 0, 0)));
        else if (m_move_vector.x < 0.f)
            copos->SetRotation(quat::fromeuler_xyz(vec3(0, 0, 180)));
        else if (m_move_vector.y > 0.f)
            copos->SetRotation(quat::fromeuler_xyz(vec3(0, 0, -90)));
        else if (m_move_vector.y < 0.f)
            copos->SetRotation(quat::fromeuler_xyz(vec3(0, 0, 90)));
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
