

#include "../oldgames.h"
#include "copmunit.h"
#include "copmlevel.h"

#include "core/json.h"
#include "engine/coposition.h"
#include "engine/controller.h"
#include "engine/tickcontext.h"

CLASS_EQUIP_CPP(CoPmUnit);

CoPmUnit::CoPmUnit() :
    m_target(nullptr),
    m_start_pos(0, 0),
    m_last_tile_coord(-1, -1),
    m_input_vector(0.f,0.f),
	m_move_vector(0.f,0.f),
    m_move_time(0.f),
    m_current_level(nullptr),
    m_hero(0),
    m_initial_life(0)
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
    m_state = eUnitState::Run;
    
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
}

void CoPmUnit::Tick( TickContext& tick_ctxt )
{
	Super::Tick(tick_ctxt);
    
    if (m_move_vector.x != 0.f || m_move_vector.y != 0.f)
        m_move_time += tick_ctxt.m_delta_seconds;
    
    // Update shader params
    {
        if (m_hero)
        {
            // Hero mouth angle
            float angle = 40.0f * F_PI / 180.0f * (0.5f + 0.5f * (float)bigfx::sin(32.0 * m_move_time));
            m_shader_param[0].x = angle;
        }
        else
        {
            // Ghost shader params
            //      m_shader_param[0] : ghost id, blip, flee, dead
            //      m_shader_param[1] : move x, move y, wave time
            m_shader_param[0].z = 0.f;
            if (m_current_level->GetGameState() == ePmGameState::GhostFlee)
            {
                float timer = m_current_level->GetGameStateTimer();
                m_shader_param[0].y = timer > 3.f ? 1.f : (0.5f + 0.5f*(float)bigfx::cos(12.f * F_2_PI * timer));
                m_shader_param[0].z = 1.f;
            }
            else if (m_current_level->GetGameState() == ePmGameState::HeroDie)
            {
                //m_shader_param[0].y = 0.f;
            }
            else // ePmGameState::Run
            {
                //m_shader_param[0].y = 1.f;
                m_shader_param[0].z = 0.f;
            }

            float ghost_wave_time = bigfx::fmod(4.f * m_move_time, 32.f);
            m_shader_param[1].z = ghost_wave_time;
        }

        m_shader_param[0].w = (m_state == eUnitState::Dead ? 1.f : 0.f);
        m_shader_param[1].xy = m_move_vector;
    }
    
    if (m_current_level->GetGameState() == ePmGameState::HeroDie)
        return; // everybody freezes while hero is dying
    
    CoPosition* copos = static_cast<CoPosition*>(GetEntityComponent("CoPosition"));
    if (!copos)
        return;

    // pacman
    //  speed regular = 1
    //  speed eating = 0.9
    // ghost
    //  speed regulat = 0.95
    //  speed fleeing = 0.5
    //  speed dead = 2
    float speed_factor = 1.f;
    if (m_hero)
    {
        if (m_current_level->m_last_ball_eaten)
        {
            speed_factor = 0.9f;
        }
    }
    else
    {
        if (m_current_level->GetGameState() == ePmGameState::GhostFlee)
        {
            speed_factor = 0.5;
        }
        if (m_state == eUnitState::Dead)
        {
            speed_factor = 2.f;
        }
    }
    
    float dmove = m_speed * speed_factor * tick_ctxt.m_delta_seconds;
    vec3 unit_pos = copos->GetPosition();
    vec2 fxy;
    ivec2 tile_coord = m_current_level->GetTileCoord(unit_pos.xy, fxy);
    PmTile& tile = m_current_level->GetTile(tile_coord.x, tile_coord.y);
    PmTileBall& tile_ball = m_current_level->GetTileBall(tile_coord.x, tile_coord.y);
    PmTileSink& tile_sink = m_current_level->GetTileSink(tile_coord.x, tile_coord.y);
    
    //////////////////////////////////////////////
    // HERO LOGIC
    if (m_hero)
    {
        // check if we eat some balls
        const float eat_margin = 0.25f;
        int ball_eaten = 0;
        vec2 ball_pos(0.f, 0.f);
        vec2 tile_fcoord((float)tile_coord.x, (float)tile_coord.y);
        if (tile_coord.x > 0 )
        {
            PmTileBall& tile_ball_left = m_current_level->GetTileBall(tile_coord.x - 1, tile_coord.y);
            if (tile_ball_left.m_right && fxy.x < eat_margin)
            {
                tile_ball_left.m_right = 0;
                ball_pos = vec2(tile_fcoord.x, tile_fcoord.y + 0.5f);
                ball_eaten++;
            }
        }
        if (tile_coord.y > 0 )
        {
            PmTileBall& tile_ball_up = m_current_level->GetTileBall(tile_coord.x, tile_coord.y - 1);
            if (tile_ball_up.m_down && fxy.y < eat_margin)
            {
                tile_ball_up.m_down = 0;
                ball_pos = vec2(tile_fcoord.x + 0.5f, tile_fcoord.y);
                ball_eaten++;
            }
        }
        if (tile_ball.m_right && fxy.x > 1.f - eat_margin)
        {
            tile_ball.m_right = 0;
            ball_pos = vec2(tile_fcoord.x + 1.f, tile_fcoord.y + 0.5f);
            ball_eaten++;
        }
        if (tile_ball.m_down && fxy.y > 1.f - eat_margin)
        {
            tile_ball.m_down = 0;
            ball_pos = vec2(tile_fcoord.x + 0.5f, tile_fcoord.y + 1.f);
            ball_eaten++;
        }
        if (tile_ball.m_center &&   fxy.x > 0.5f - eat_margin && fxy.x < 0.5f + eat_margin &&
                                    fxy.y > 0.5f - eat_margin && fxy.y < 0.5f + eat_margin)
        {
            tile_ball.m_center = 0;
            if (tile_ball.m_big)
                m_current_level->RequestGameStateChange(ePmGameState::GhostFlee);
            ball_pos = vec2(tile_fcoord.x + 0.5f, tile_fcoord.y + 0.5f);
            ball_eaten++;
        }
        
        if (ball_eaten > 0)
            m_current_level->SetBallEatenAtPos(ball_pos);
        else
        {
            vec2 delta_pos = m_current_level->m_last_ball_eaten_pos - unit_pos.xy;
            if (bigfx::abs(delta_pos.x) > 1.f || bigfx::abs(delta_pos.y) > 1.f)
                m_current_level->ResetBallEaten();
        }

        m_state = eUnitState::Run;
    }
    //////////////////////////////////////////////
    // GHOST AI LOGIC
    else
    {
        const vec2 dir_input[4] = { vec2(-1, 0), vec2(1, 0), vec2(0,1), vec2(0, -1) };

        if (m_state == eUnitState::Run)
        {
            // check whether we are not moving anymore
            if (m_move_vector == vec2::zero)
            {
                int32 dir_idx = bigfx::rand() % 4;
                for (int32 i = 0; i < 4; i++)
                {
                    const int32 d = (dir_idx + i) & 3;
                    if (tile.m_dir[d] != PmTile::eEdgeType::Wall)
                    {
                        m_input_vector = dir_input[d];
                        break;
                    }
                }
            }
            // otherwise if we have changed tile, we can try to switch direction
            else if (tile_coord != m_last_tile_coord)
            {
                int32 d = bigfx::rand() % 4;

                // if target is in view, try to follow it instead
                if (m_target)
                {
                    int32 target_d = m_current_level->CanViewPosition(tile_coord, m_target->m_last_tile_coord);
                    if (target_d != INDEX_NONE)
                    {
                        if (m_current_level->GetGameState() == ePmGameState::GhostFlee)
                            d = m_current_level->ReverseDir(target_d);
                        else
                            d = target_d;
                    }
                }

                if (tile.m_dir[d] != PmTile::eEdgeType::Wall && dir_input[d] != -m_move_vector)
                {
                    m_input_vector = dir_input[d];
                }

            }

            // check ghost collision with target
            const float coll_margin = 0.45f;
            CoPosition* cotargetpos = static_cast<CoPosition*>(m_target->GetEntityComponent("CoPosition"));
            vec3 target_pos = cotargetpos->GetPosition();
            float dist_pos = bigfx::length(unit_pos.xy - target_pos.xy);
            if (dist_pos < 1.f - coll_margin)
            {
                if (m_current_level->GetGameState() == ePmGameState::Run)
                {
                    m_target->m_state = eUnitState::Dead;
                    m_current_level->RequestGameStateChange(ePmGameState::HeroDie);
                }
                else if (m_current_level->GetGameState() == ePmGameState::GhostFlee)
                {
                    m_state = eUnitState::Dead;
                }
            }
        }
        else if (m_state == eUnitState::Dead)
        {
            if (tile.m_type == PmTile::TypeSink)
            {
                m_state = eUnitState::Run;
            }
            else
            {
                // follow the path to initial sinks
                for (int i = 0; i < 4; i++)
                {
                    if (tile_sink.m_dir[i])
                    {
                        m_input_vector = dir_input[i];
                        break;
                    }
                }
            }
        }
    }
    
    // check if we can consume any input
    if (m_input_vector.x < 0.f)
    {
        if ((fxy.y >= 0.5f - dmove && fxy.y <= 0.5f + dmove) && (fxy.x > 0.5f || tile.m_left != PmTile::eEdgeType::Wall))
        {
            unit_pos.y -= fxy.y - 0.5f;
            m_move_vector = vec2(-1.f, 0.f);
            m_input_vector = vec2::zero;
        }
    }
    else if (m_input_vector.x > 0.f)
    {
        if ((fxy.y >= 0.5f - dmove && fxy.y <= 0.5f + dmove) && (fxy.x < 0.5f || tile.m_right != PmTile::eEdgeType::Wall))
        {
            unit_pos.y -= fxy.y - 0.5f;
            m_move_vector = vec2(1.f, 0.f);
            m_input_vector = vec2::zero;
        }
    }
    else if (m_input_vector.y < 0.f)
    {
        if ((fxy.x >= 0.5f - dmove && fxy.x <= 0.5f + dmove) && (fxy.y < 0.5f || tile.m_down != PmTile::eEdgeType::Wall))
        {
            unit_pos.x -= fxy.x - 0.5f;
            m_move_vector = vec2(0.f, -1.f);
            m_input_vector = vec2::zero;
        }
    }
    else if (m_input_vector.y > 0.f)
    {
        if ((fxy.x >= 0.5f - dmove && fxy.x <= 0.5f + dmove) && (fxy.y > 0.5f || tile.m_up != PmTile::eEdgeType::Wall))
        {
            unit_pos.x -= fxy.x - 0.5f;
            m_move_vector = vec2(0.f, 1.f);
            m_input_vector = vec2::zero;
        }
    }

    unit_pos.x += m_move_vector.x * dmove;
    unit_pos.y -= m_move_vector.y * dmove;
    
    // check walls
    float dx = bigfx::fract(unit_pos.x) - 0.5f;
    float dy = bigfx::fract(unit_pos.y) - 0.5f;
    if (m_move_vector.x < 0.f && tile.m_left == PmTile::eEdgeType::Wall && dx <= 0.0f)
    {
        unit_pos.x -= dx;
        m_move_vector = vec2::zero;
    }
    else if (m_move_vector.x > 0.f && tile.m_right == PmTile::eEdgeType::Wall && dx >= 0.0f)
    {
        unit_pos.x -= dx;
        m_move_vector = vec2::zero;
    }
    else if (m_move_vector.y < 0.f && tile.m_down == PmTile::eEdgeType::Wall && dy >= 0.0f)
    {
        unit_pos.y -= dy;
        m_move_vector = vec2::zero;
    }
    else if (m_move_vector.y > 0.f && tile.m_up == PmTile::eEdgeType::Wall && dy <= 0.0f)
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
