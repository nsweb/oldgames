#include "../oldgames.h"
#include "copmlevel.h"
#include "copmunit.h"
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
#include "gfx/drawutils.h"
#include "../game/ogworld.h"

CLASS_EQUIP_CPP(CoPmLevel);

CoPmLevel::CoPmLevel() :
    m_tile_shader(BGFX_INVALID_HANDLE),
    m_ball_shader(BGFX_INVALID_HANDLE),
    m_last_ball_eaten(false),
    m_state_change_request(false),
    m_current_game_state(ePmGameState::Run),
    m_pending_game_state(ePmGameState::Run),
    m_state_timer(0.f)
{

}

CoPmLevel::~CoPmLevel()
{

}

void CoPmLevel::Create( Entity* owner, class json::Object* proto )
{
	Super::Create( owner, proto );
    
    LoadShaders();
    CreateBuffers();

    //Array<String> layout {
    //    " - - - - - - ",
    //    "|O    |    .|",
    //    "   -     -   ",
    //    "| |       |X|",
    //    " - - - - - - " };

    //Array<String> layout {
    //    " - - - - - - ",
    //    "|O    |    .|",
    //    "   -     -   ",
    //    "| |       | |",
    //    "       -     ",
    //    "|   |X X|   |",
    //    "     - -     ",
    //    "|.|       |.|",
    //    "   - - - -   ",
    //    "|    .|.    |",
    //    "   - - - -   ",
    //    "|.         .|",
    //    " - - - - - - " };

    Array<String> layout {
    " - - - - - - - - - - - - ",
    "|     |      X   .     .|",
    "   -     - -   - -       ",
    "| |       |    .    |   |",
    "     - = -   -     -     ",
    "|   |1 2 1|   |   |     |",
    "     - - -       - - -   ",
    "| |      .| | |         |",
    "   - - - -               ",
    "|    .|.    |  .|    .  |",
    "   - - - -     - - -     ",
    "|.         .           .|",
    " - - - - - - - - - - - - "};
    
    //const char layout1[] =
    //" - - - - - - - - - - - - "
    //"|O    |                .|"
    //"   -     - - - - -       "
    //"| |       |    . .  |   |"
    //"       -           -     "
    //"|   |X X X X      |     |"
    //"     - -  -      - - -   "
    //"|.|       |.|    .      |"
    //"   - - - -               "
    //"|    .|.    |  .|    .  |"
    //"   - - - -     - - -     "
    //"|.         .           .|"
    //" - - - - - - - - - - - - ";
    
    //const char layout1[] =
    //" - - - - - - - - - - - - "
    //"|        O             .|"
    //"   -     - - - - -       "
    //"| |       |    . .  |   |"
    //"       -           -     "
    //"|      X      |   |X    |"
    //"     - -  -      - - -   "
    //"|.|        .| |  .      |"
    //"   - - - -               "
    //"|    .|.    |  .|    .  |"
    //"   - - - -     - - -     "
    //"|.         .           .|"
    //" - - - - - - - - - - - - ";

    const int h = layout.size();
    int w = INT_MAX;
    for (int j=0; j<h; j++)
        w = bigfx::min(layout[j].Len(), w);
    m_tile_dim = ivec2(w/2, h/2);

    ivec2 layout_dim = ivec2((m_tile_dim.x * 2 + 1), (m_tile_dim.y * 2 + 1));
    m_tiles.resize(m_tile_dim.x * m_tile_dim.y);
    m_tile_balls.resize(m_tile_dim.x * m_tile_dim.y);
    m_tile_sinks.resize(m_tile_dim.x * m_tile_dim.y);
    m_ghost_starts.clear();

    auto EdgeFunc = [](uint8 t)
    {
        switch (t)
        {
        case '|':
        case '-':
            return PmTile::eEdgeType::Wall;
        case '"':
        case '=':
            return PmTile::eEdgeType::GhostSink;
        case '{':
        case '}':
        case '¨':
            return PmTile::eEdgeType::Portal;
        };
        return PmTile::eEdgeType::None;
    };

    for(int j = 0; j < m_tile_dim.y; j++)
    {
        int jL = 1 + 2*j;
        for(int i = 0; i < m_tile_dim.x; i++)
        {
            int iL = 1 + 2*i;
            int tile_type = 0;
            PmTile& tile = GetTile(i, j);
            PmTileBall& tile_ball = GetTileBall(i, j);
            char val = layout[jL][iL];
            if (val == 'X')
            {
                m_hero_start = ivec2(i, j);
            }
            else if (val >= '0' && val <= '9')
            {
                int ghost_count = (int)(val - '0');
                m_ghost_starts.push_back(ivec3(i, j, ghost_count));
                tile_type |= PmTile::TypeSink;
            }
            
            tile.m_left     = EdgeFunc( layout[jL][(iL - 1)] );
            tile.m_right    = EdgeFunc( layout[jL][(iL + 1)] );
            tile.m_up       = EdgeFunc( layout[(jL - 1)][iL] );
            tile.m_down     = EdgeFunc( layout[(jL + 1)][iL] );
            tile.m_type     = tile_type;
            
            tile_ball.m_big       = val == '.' ? 1 : 0;
            tile_ball.m_center    = 1;
            tile_ball.m_right     = (tile.m_right == PmTile::eEdgeType::None ? 1 : 0);
            tile_ball.m_down      = (tile.m_down == PmTile::eEdgeType::None ? 1 : 0);

            if ((tile_type & PmTile::TypeSink) != 0)
            {
                tile_ball.m_data = 0;
            }
        }
    }

    // sinks
    {
        Array<ivec2> sink_opens;
        sink_opens.reserve(m_tile_dim.x* m_tile_dim.y);
        for (int j = 0; j < m_tile_dim.y; j++)
        {
            for (int i = 0; i < m_tile_dim.x; i++)
            {
                PmTileSink& tile_sink = GetTileSink(i, j);
                tile_sink.m_data = 0;

                const PmTile& tile = GetTile(i, j);
                for (int ii = 0; ii < 4; ii++)
                {
                    if (tile.m_dir[ii] == PmTile::eEdgeType::GhostSink)
                    {
                        tile_sink.m_dir[ii] = 1;
                        sink_opens.push_back(ivec2(i, j));
                        break;
                    }
                }
            }
        }
        for (int k = 0; k < sink_opens.size(); k++)
        {
            ivec2 ij = sink_opens[k];
            const PmTile& tile = GetTile(ij.x, ij.y);
            PmTileSink& tile_sink = GetTileSink(ij.x, ij.y);

            // Check tile neighbors
            if (ij.x > 0)
            {
                PmTile& tile_left = GetTile(ij.x - 1, ij.y);
                if (tile_left.m_right != PmTile::eEdgeType::Wall)
                {
                    PmTileSink& tile_sink_left = GetTileSink(ij.x - 1, ij.y);
                    if (tile_sink_left.m_data == 0) // has tile already been tagged ?
                    {
                        tile_sink_left.m_right = 1;
                        sink_opens.push_back(ivec2(ij.x - 1, ij.y));
                    }
                }
            }
            if (ij.y > 0)
            {
                PmTile& tile_up = GetTile(ij.x, ij.y - 1);
                if (tile_up.m_down != PmTile::eEdgeType::Wall)
                {
                    PmTileSink& tile_sink_up = GetTileSink(ij.x, ij.y - 1);
                    if (tile_sink_up.m_data == 0) // has tile already been tagged ?
                    {
                        tile_sink_up.m_down = 1;
                        sink_opens.push_back(ivec2(ij.x, ij.y - 1));
                    }
                }
            }
            if (ij.x < m_tile_dim.x - 1)
            {
                if (tile.m_right != PmTile::eEdgeType::Wall)
                {
                    PmTileSink& tile_sink_right = GetTileSink(ij.x + 1, ij.y);
                    if (tile_sink_right.m_data == 0) // has tile already been tagged ?
                    {
                        tile_sink_right.m_left = 1;
                        sink_opens.push_back(ivec2(ij.x + 1, ij.y));
                    }
                }
            }
            if (ij.y < m_tile_dim.y - 1)
            {
                if (tile.m_down != PmTile::eEdgeType::Wall)
                {
                    PmTileSink& tile_sink_down = GetTileSink(ij.x, ij.y + 1);
                    if (tile_sink_down.m_data == 0) // has tile already been tagged ?
                    {
                        tile_sink_down.m_up = 1;
                        sink_opens.push_back(ivec2(ij.x, ij.y + 1));
                    }
                }
            }
        }
    }
    
    String ghost_name;
    m_ghosts.clear();
    int gidx = 0;
    for(int i = 0; i < m_ghost_starts.size(); i++)
    {
        const int ghost_count = m_ghost_starts[i].z;
        for (int g = 0; g < ghost_count; g++, gidx++)
        {
            ghost_name = String::Printf("ghost_%d", gidx);
            Entity* ent_ghost = EntityManager::GetStaticInstance()->CreateEntityFromJson("../data/pacman/ghost.json", ghost_name.c_str());
            CoPmUnit* ghost = static_cast<CoPmUnit*>(ent_ghost->GetComponent("CoPmUnit"));
            ghost->m_start_pos = m_ghost_starts[i].xy;
            ghost->m_shader_param[0].x = (float)i;
            ghost->m_shader_param[0].y = (float)0;
            m_ghosts.push_back(ghost);
        }
    }
}

void CoPmLevel::GetLevelBounds(vec2& bmin, vec2& bmax) const
{
    bmin = vec2(0.f,0.f);
    bmax = vec2((float)m_tile_dim.x, (float)m_tile_dim.y);
}

ivec2 CoPmLevel::GetTileCoord(vec2 pos, vec2& frac_xy)
{
    int ix = (int)pos.x;
    int iy = (int)pos.y;
    frac_xy = vec2(pos.x - (float)ix, pos.y - (float)iy);
    
	int i = bigfx::clamp(ix, 0, m_tile_dim.x - 1);
	int j = bigfx::clamp(iy, 0, m_tile_dim.y - 1);
    
	return ivec2(i, j);
}

void CoPmLevel::BeginPlay(bool new_game)
{
    m_current_game_state = ePmGameState::Run;
    m_pending_game_state = ePmGameState::Run;
    
    CoPmUnit* hero_unit = nullptr;
    if (m_hero)
    {
        hero_unit = static_cast<CoPmUnit*>( m_hero->GetComponent("CoPmUnit") );
        hero_unit->m_start_pos = m_hero_start;
        hero_unit->BeginPlay(this, new_game);
    }

    int gidx = 0;
    for (int i = 0; i < m_ghost_starts.size(); i++)
    {
        const int ghost_count = m_ghost_starts[i].z;
        for (int g = 0; g < ghost_count; g++, gidx++)
        {
            m_ghosts[gidx]->m_start_pos = m_ghost_starts[i].xy;
            m_ghosts[gidx]->m_target = hero_unit;
            m_ghosts[gidx]->BeginPlay(this, new_game);
        }
    }
}

void CoPmLevel::OnControllerInput( Camera* camera, ControllerInput const& input )
{
    if (m_hero)
    {
		CoPmUnit* unit = static_cast<CoPmUnit*>( m_hero->GetComponent("CoPmUnit") );
		unit->OnControllerInput(input);
    }
}

void CoPmLevel::Destroy()
{
    bgfx::destroy(m_u_tile_dim);
    bgfx::destroy(m_tile_shader);
    bgfx::destroy(m_ball_shader);
    DestroyBuffers();

	Super::Destroy();
}

void CoPmLevel::AddToWorld()
{
    for (int i = 0; i < m_ghosts.size(); i++)
        EntityManager::GetStaticInstance()->AddEntityToWorld( m_ghosts[i]->GetEntity() );
        
	Super::AddToWorld();
}

void CoPmLevel::RemoveFromWorld()
{
    for (int i = 0; i < m_ghosts.size(); i++)
        EntityManager::GetStaticInstance()->RemoveEntityFromWorld( m_ghosts[i]->GetEntity() );
    
	Super::RemoveFromWorld();
}

void CoPmLevel::Tick( TickContext& tick_ctxt )
{
    m_current_game_state = m_pending_game_state;
    
    if (m_state_change_request)
    {
        m_state_timer = 6.f;
        switch (m_current_game_state) {
            case ePmGameState::HeroDie:
                OGWorld::GetStaticInstance()->RequestTransition(eTransitionType::PlayerDies);
                break;
        }
    }
    
    m_state_timer -= tick_ctxt.m_delta_seconds;
    if (m_state_timer <= 0.f)
    {
        switch (m_current_game_state) {
            case ePmGameState::GhostFlee:
                m_current_game_state = ePmGameState::Run;
                break;
            case ePmGameState::HeroDie:
                m_current_game_state = ePmGameState::Run;
                break;
                
            default:
                break;
        }
        m_state_timer = 0.f;
    }
    
    m_state_change_request = false;
}

void CoPmLevel::_Render( RenderContext& render_ctxt )
{
    //return;
#if 0
    u8vec4 col_white = u8vec4(255,255,255,255);
    u8vec4 col_red = u8vec4(255,0,0,255);
    u8vec4 col_green = u8vec4(0,255,0,255);
    DrawUtils::GetStaticInstance()->PushSegment(vec3(0,0,0), vec3(5,0, 0), col_red, col_red);
    DrawUtils::GetStaticInstance()->PushSegment(vec3(0,0,0), vec3(0,5, 0), col_green, col_green);
    DrawUtils::GetStaticInstance()->PushSegment(vec3(0,0,0), vec3(0,0, 5), col_white, col_white);
#endif
    
    //mat4 cam_to_world_mat(render_ctxt.m_view.m_transform.GetRotation(), render_ctxt.m_view.m_transform.GetTranslation(), (float)render_ctxt.m_view.m_transform.GetScale());
    //mat4 view_mat = bigfx::inverse(cam_to_world_mat);
    mat4 world_mat(quat(1.f), vec3(0.f, 0.f, 0.f), 1.f);

    // tiles
    {
        bgfx::setTransform(&world_mat.v0);
        bgfx::setViewTransform(0, &render_ctxt.m_view_mat.v0, &render_ctxt.m_proj_mat.v0);

        vec4 tile_dim4((float)m_tile_dim.x, (float)m_tile_dim.y, 0.f, 0.f);
        bgfx::setUniform(m_u_tile_dim, &tile_dim4);

        const uint32 instance_stride = sizeof(vec4);
        uint32 num_tiles = m_tiles.size();
        uint32 avail_tiles = bgfx::getAvailInstanceDataBuffer(num_tiles, instance_stride);
        if (num_tiles <= avail_tiles)
        {
            bgfx::InstanceDataBuffer idb;
            bgfx::allocInstanceDataBuffer(&idb, num_tiles, instance_stride);
            vec4* data = (vec4*)idb.data;

            for (uint32 ii = 0; ii < num_tiles; ++ii)
            {
                data[ii].x = (uint8)m_tiles[ii].m_dir[0];
                data[ii].y = (uint8)m_tiles[ii].m_dir[1];
                data[ii].z = (uint8)m_tiles[ii].m_dir[2];
                data[ii].w = (uint8)m_tiles[ii].m_dir[3];
            }

            // Set vertex and index buffer.
            bgfx::setVertexBuffer(0, m_vbh_quad);
            bgfx::setIndexBuffer(m_ibh_quad);

            // Set instance data buffer.
            bgfx::setInstanceDataBuffer(&idb);

            uint64 state = (0
                | BGFX_STATE_WRITE_RGB
                | BGFX_STATE_WRITE_A
                //| BGFX_STATE_WRITE_Z
                //| BGFX_STATE_DEPTH_TEST_LESS
                | BGFX_STATE_BLEND_ALPHA
                | BGFX_STATE_BLEND_NORMAL
                | BGFX_STATE_CULL_CW
                | BGFX_STATE_MSAA
                );
            bgfx::setState(state);
            //bgfx::setState(BGFX_STATE_DEFAULT);
            bgfx::submit(0, m_tile_shader);
        }
    }
    
    // balls
    {
        bgfx::setTransform(&world_mat.v0);
        bgfx::setViewTransform(0, &render_ctxt.m_view_mat.v0, &render_ctxt.m_proj_mat.v0);

        vec4 tile_dim4((float)m_tile_dim.x, (float)m_tile_dim.y, 0.f, 0.f);
        bgfx::setUniform(m_u_tile_dim, &tile_dim4);

        const uint32 instance_stride = sizeof(vec4);
        uint32 num_balls = m_tile_balls.size();
        uint32 avail_balls = bgfx::getAvailInstanceDataBuffer(num_balls, instance_stride);
        if (num_balls <= avail_balls)
        {
            bgfx::InstanceDataBuffer idb;
            bgfx::allocInstanceDataBuffer(&idb, num_balls, instance_stride);
            vec4* data = (vec4*)idb.data;

            for (uint32 ii = 0; ii < num_balls; ++ii)
            {
                data[ii].x = m_tile_balls[ii].m_big;
                data[ii].y = m_tile_balls[ii].m_center;
                data[ii].z = m_tile_balls[ii].m_right;
                data[ii].w = m_tile_balls[ii].m_down;
            }

            // Set vertex and index buffer.
            bgfx::setVertexBuffer(0, m_vbh_quad);
            bgfx::setIndexBuffer(m_ibh_quad);

            // Set instance data buffer.
            bgfx::setInstanceDataBuffer(&idb);

            uint64 state = (0
                | BGFX_STATE_WRITE_RGB
                | BGFX_STATE_WRITE_A
                //| BGFX_STATE_WRITE_Z
                //| BGFX_STATE_DEPTH_TEST_LESS
                | BGFX_STATE_BLEND_ALPHA
                | BGFX_STATE_BLEND_NORMAL
                | BGFX_STATE_CULL_CW
                | BGFX_STATE_MSAA
                );
            bgfx::setState(state);

            //bgfx::setState(BGFX_STATE_DEFAULT);
            bgfx::submit(0, m_ball_shader);
        }
    }
}

void CoPmLevel::LoadShaders()
{
    m_tile_shader = loadProgram("tile_vs", "tile_fs");
    m_ball_shader = loadProgram("ball_vs", "ball_fs");

    m_u_tile_dim = bgfx::createUniform("u_tile_dim", bgfx::UniformType::Vec4);
}

void CoPmLevel::CreateBuffers()
{
    // tile quad
    static vec2 tile_vertices[] = { vec2(0.f,0.f), vec2(1.f,0.f), vec2(1.f,1.f), vec2(0.f,1.f) };
    static uint16 idx_data[] = {
        0,2,1, 0,3,2
    };

    m_vbh_quad = bgfx::createVertexBuffer(bgfx::makeRef(tile_vertices, sizeof(tile_vertices)),
        Draw::QuadVertex::ms_layout);

    m_ibh_quad = bgfx::createIndexBuffer(
        // Static data can be passed with bgfx::makeRef
        bgfx::makeRef(idx_data, sizeof(idx_data))
    );
}

void CoPmLevel::DestroyBuffers()
{
    bgfx::destroy(m_vbh_quad);
    bgfx::destroy(m_ibh_quad);
}

int32 CoPmLevel::CanViewPosition(ivec2 from, ivec2 to) const
{
    if (to.y == from.y)
    {
        if (to.x == from.x)
            return 1;
            
        int32 a = bigfx::min(to.x, from.x);
        int32 b = bigfx::max(to.x, from.x);
        for(int32 t = a; t < b; t++)
        {
            if (GetTile(t, from.y).m_right == PmTile::eEdgeType::Wall)
                return INDEX_NONE;
        }
        return to.x > from.x ? 1 /*right*/ : 0 /*left*/;
    
    }
    else if (to.x == from.x)
    {
        int32 a = bigfx::min(to.y, from.y);
        int32 b = bigfx::max(to.y, from.y);
        for(int32 t = a; t < b; t++)
        {
            if (GetTile(from.x, t).m_down == PmTile::eEdgeType::Wall)
                return INDEX_NONE;
        }
        return to.y > from.y ? 3 /*down*/ : 2 /*up*/;
    }
    
    return INDEX_NONE;
}

int32 CoPmLevel::ReverseDir(int32 dir) const
{
    switch (dir) {
        case 0: return 1; break;
        case 1: return 0; break;
        case 2: return 3; break;
        case 3: return 2; break;
        default:
            break;
    }
    return -1;
}

