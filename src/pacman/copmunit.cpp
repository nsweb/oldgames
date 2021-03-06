

#include "../oldgames.h"
#include "copmunit.h"
#include "copmlevel.h"

#include "core/json.h"
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
    m_target(nullptr),
    m_start_pos(0, 0),
    m_last_tile_coord(-1, -1),
    m_input_vector(0.f,0.f),
	m_move_vector(0.f,0.f),
    m_current_level(nullptr),
    m_hero(0),
    m_initial_life(0),
    m_is_weak(false)
{

}

CoPmUnit::~CoPmUnit()
{

}

void CoPmUnit::Create( Entity* owner, class json::Object* proto )
{
	Super::Create( owner, proto );
    
    json::TokenIdx ent_tok = proto->GetToken( "entity", json::OBJECT );
    json::TokenIdx unit_tok = proto->GetToken( "unit", json::OBJECT, ent_tok );
    if(unit_tok != INDEX_NONE )
    {
        json::TokenIdx param_tok = proto->GetToken( "hero", json::PRIMITIVE, unit_tok);
        if( param_tok != INDEX_NONE )
            m_hero = proto->GetIntValue( param_tok, m_hero);
        param_tok = proto->GetToken( "life", json::PRIMITIVE, unit_tok);
        if( param_tok != INDEX_NONE )
            m_initial_life = proto->GetIntValue( param_tok, m_initial_life);
    }
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

void CoPmUnit::BeginPlay(CoPmLevel* level, bool new_game)
{
    m_current_level = level;
    
    // reset unit pos
    CoPosition* copos = static_cast<CoPosition*>(GetEntityComponent("CoPosition"));
    if (copos)
    {
        // top left corner
        if (!m_hero || new_game)
            copos->SetPosition(vec3(m_start_pos.x + 0.5f, m_start_pos.y + 0.5f, 0));
    }
    
    m_shader_param[0].y = 1.f; // alpha
    m_input_vector = vec2::zero;
    m_move_vector = vec2::zero;
    m_is_weak = false;
}

void CoPmUnit::Tick( TickContext& tick_ctxt )
{
	Super::Tick(tick_ctxt);
    
    static float global_time = 0.f;
    if (m_move_vector.x != 0.f || m_move_vector.y != 0.f)
        global_time += tick_ctxt.m_delta_seconds;
    
    if (m_hero)
    {
        float angle = 40.0 * F_PI / 180.0 * (0.5 + 0.5 * bigball::sin(8.0*global_time));
        m_shader_param[0].x = angle;
    }
    else
    {
        if (m_current_level->GetGameState() == ePmGameState::GhostFlee)
        {
            float timer = m_current_level->GetGameStateTimer();
            m_shader_param[0].y = 0.6 + 0.4f * bigball::cos(12.f * F_2_PI * timer);
        }
        else if (m_current_level->GetGameState() == ePmGameState::HeroDie)
        {
            m_shader_param[0].y = 0.f;
        }
        else
        {
            m_shader_param[0].y = 1.f;
        }
        
        static float ghost_time = 0.f;
        ghost_time = bigball::fmod(ghost_time + tick_ctxt.m_delta_seconds, 32.f);
        m_shader_param[0].w = ghost_time;
    }
    
    if (m_current_level->GetGameState() == ePmGameState::HeroDie)
        return; // everybody freezes while hero is dying
    
    CoPosition* copos = static_cast<CoPosition*>(GetEntityComponent("CoPosition"));
    if (!copos)
        return;
    
    float dmove = m_speed * tick_ctxt.m_delta_seconds;
    vec3 unit_pos = copos->GetPosition();
    vec2 fxy;
    ivec2 tile_coord = m_current_level->GetTileCoord(unit_pos.xy, fxy);
    PmTile& tile = m_current_level->GetTile(tile_coord.x, tile_coord.y);
    PmTileBall& tile_ball = m_current_level->GetTileBall(tile_coord.x, tile_coord.y);
    
    // ai logic
    if (!m_hero )
    {
        const vec2 dir_input[4] = { vec2(-1, 0), vec2(1, 0), vec2(0,1), vec2(0, -1) };
        // check whether we are not moving anymore
        if (m_move_vector == vec2::zero)
        {
            int32 dir_idx = bigball::rand() % 4;
            for (int32 i = 0; i < 4; i++)
            {
                const int32 d = (dir_idx + i) & 3;
                if (tile.m_dir[d])
                {
                    m_input_vector = dir_input[d];
                    break;
                }
            }
        }
        // otherwise if we have changed tile, we can try to switch direction
        else if (tile_coord != m_last_tile_coord)
        {
            int32 d = bigball::rand() % 4;
            
            // if target is in view, try to follow it instead
            if (m_target)
            {
                int32 target_d = m_current_level->CanViewPosition(tile_coord, m_target->m_last_tile_coord);
                if (target_d != INDEX_NONE)
                {
                    if(m_current_level->GetGameState() == ePmGameState::GhostFlee)
                        d = m_current_level->ReverseDir(target_d);
                    else
                        d = target_d;
                }
            }
            
            if (tile.m_dir[d] && dir_input[d] != -m_move_vector )
            {
                m_input_vector = dir_input[d];
            }
            
        }
    }
    
    // check if we eat some balls
    if (m_hero)
    {
        const float eat_margin = 0.25f;
        int ball_eaten = 0;
        if (tile_coord.x > 0 )
        {
            PmTileBall& tile_ball_left = m_current_level->GetTileBall(tile_coord.x - 1, tile_coord.y);
            if (tile_ball_left.m_right && fxy.x < eat_margin)
            {
                tile_ball_left.m_right = 0;
                ball_eaten++;
            }
        }
        if (tile_coord.y > 0 )
        {
            PmTileBall& tile_ball_up = m_current_level->GetTileBall(tile_coord.x, tile_coord.y - 1);
            if (tile_ball_up.m_down && fxy.y < eat_margin)
            {
                tile_ball_up.m_down = 0;
                ball_eaten++;
            }
        }
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
            if (tile_ball.m_big)
                m_current_level->RequestGameStateChange(ePmGameState::GhostFlee);
            ball_eaten++;
        }
        
        if (ball_eaten > 0)
            m_current_level->NeedBallRedraw();
    }
    else
    {
        // ai logic : check collision
        if (tile_coord == m_target->m_last_tile_coord)
        {
            if (m_current_level->GetGameState() == ePmGameState::Run)
            {
                m_current_level->RequestGameStateChange(ePmGameState::HeroDie);
            }
            else if (m_current_level->GetGameState() == ePmGameState::GhostFlee)
            {
                m_is_weak = true;
            }
        }
        
    }
    
    // check if we can consume any input
    if (m_input_vector.x < 0.f)
    {
        if ((fxy.y >= 0.5f - dmove && fxy.y <= 0.5f + dmove) && (fxy.x > 0.5f || tile.m_left))
        {
            unit_pos.y -= fxy.y - 0.5f;
            m_move_vector = vec2(-1.f, 0.f);
            m_input_vector = vec2::zero;
        }
    }
    else if (m_input_vector.x > 0.f)
    {
        if ((fxy.y >= 0.5f - dmove && fxy.y <= 0.5f + dmove) && (fxy.x < 0.5f || tile.m_right))
        {
            unit_pos.y -= fxy.y - 0.5f;
            m_move_vector = vec2(1.f, 0.f);
            m_input_vector = vec2::zero;
        }
    }
    else if (m_input_vector.y < 0.f)
    {
        if ((fxy.x >= 0.5f - dmove && fxy.x <= 0.5f + dmove) && (fxy.y < 0.5f || tile.m_down))
        {
            unit_pos.x -= fxy.x - 0.5f;
            m_move_vector = vec2(0.f, -1.f);
            m_input_vector = vec2::zero;
        }
    }
    else if (m_input_vector.y > 0.f)
    {
        if ((fxy.x >= 0.5f - dmove && fxy.x <= 0.5f + dmove) && (fxy.y > 0.5f || tile.m_up))
        {
            unit_pos.x -= fxy.x - 0.5f;
            m_move_vector = vec2(0.f, 1.f);
            m_input_vector = vec2::zero;
        }
    }

    unit_pos.x += m_move_vector.x * dmove;
    unit_pos.y -= m_move_vector.y * dmove;
    
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
    if (m_hero)
    {
        if (m_move_vector.x > 0.f)
            copos->SetRotation(quat::fromeuler_xyz(vec3(0, 0, 0)));
        else if (m_move_vector.x < 0.f)
            copos->SetRotation(quat::fromeuler_xyz(vec3(0, 0, 180)));
        else if (m_move_vector.y > 0.f)
            copos->SetRotation(quat::fromeuler_xyz(vec3(0, 0, -90)));
        else if (m_move_vector.y < 0.f)
            copos->SetRotation(quat::fromeuler_xyz(vec3(0, 0, 90)));
    }
    
    m_last_tile_coord = tile_coord;
    
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
