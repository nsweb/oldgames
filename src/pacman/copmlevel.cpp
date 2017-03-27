

#include "../oldgames.h"
#include "copmlevel.h"
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
#include "copmunit.h"

CLASS_EQUIP_CPP(CoPmLevel);

CoPmLevel::CoPmLevel() :
    m_tile_shader(nullptr),
    m_ball_shader(nullptr),
    m_need_ball_redraw(false)
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
    
    m_tile_dim = ivec2(6, 6);

    const char layout1[] =
        " - - - - - - "
        "|O    |    .|"
        "   -     -   "
        "| |       | |"
        "       -     "
        "|   |X X|   |"
        "     - -     "
        "|.|       |.|"
        "   - - - -   "
        "|    .|.    |"
        "   - - - -   "
        "|.         .|"
        " - - - - - - ";
    ivec2 layout_dim = ivec2((m_tile_dim.x * 2 + 1), (m_tile_dim.y * 2 + 1));
    BB_ASSERT(sizeof(layout1) == (layout_dim.x * layout_dim.y + 1));
    
    m_tiles.resize(m_tile_dim.x * m_tile_dim.y);
    m_tile_balls.resize(m_tile_dim.x * m_tile_dim.y);
    m_ghost_starts.clear();

    for(int j = 0; j < m_tile_dim.y; j++)
    {
        int jL = 1 + 2*j;
        for(int i = 0; i < m_tile_dim.x; i++)
        {
            int iL = 1 + 2*i;
            PmTile& tile = GetTile(i, j);
            PmTileBall& tile_ball = GetTileBall(i, j);
            char val = layout1[jL * layout_dim.x + iL];
            if (val == 'O')
                m_hero_start = ivec2(i, j);
            else if (val == 'X')
                m_ghost_starts.push_back( ivec2(i, j) );

            tile.m_left     = layout1[jL * layout_dim.x + (iL - 1)] == '|' ? 0 : 1;
            tile.m_right    = layout1[jL * layout_dim.x + (iL + 1)] == '|' ? 0 : 1;
            tile.m_up       = layout1[(jL - 1) * layout_dim.x + iL] == '-' ? 0 : 1;
            tile.m_down     = layout1[(jL + 1) * layout_dim.x + iL] == '-' ? 0 : 1;
            
            tile_ball.m_big       = val == '.' ? 1 : 0;
            tile_ball.m_center    = 1;
            tile_ball.m_right     = tile.m_right;
            tile_ball.m_down      = tile.m_down;
        }
    }
    
    String ghost_name;
    m_ghosts.resize(m_ghost_starts.size());
    for(int i = 0; i < m_ghost_starts.size(); i++)
    {
        ghost_name = String::Printf("ghost_%d", i);
        Entity* ent_ghost = EntityManager::GetStaticInstance()->CreateEntityFromJson("../data/pacman/ghost.json", ghost_name.c_str());
        m_ghosts[i] = static_cast<CoPmUnit*>( ent_ghost->GetComponent("CoPmUnit") );
        m_ghosts[i]->m_start_pos = m_ghost_starts[i];
        m_ghosts[i]->m_shader_param = (float)i;
    }
    
    glBindBuffer(GL_ARRAY_BUFFER, m_vbuffers[eVBTileWall]);
    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)m_tiles.size() * sizeof(PmTile), (GLvoid*)m_tiles.Data(), GL_STATIC_DRAW );

    glBindBuffer(GL_ARRAY_BUFFER, m_vbuffers[eVBTileBall]);
    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)m_tile_balls.size() * sizeof(PmTileBall), (GLvoid*)m_tile_balls.Data(), GL_STATIC_DRAW );
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
    
	int i = bigball::clamp(ix, 0, m_tile_dim.x - 1);
	int j = bigball::clamp(iy, 0, m_tile_dim.y - 1);
    
	return ivec2(i, j);
}

void CoPmLevel::BeginPlay()
{
    CoPmUnit* hero_unit = nullptr;
    if (m_hero)
    {
        hero_unit = static_cast<CoPmUnit*>( m_hero->GetComponent("CoPmUnit") );
        hero_unit->m_start_pos = m_hero_start;
        hero_unit->BeginPlay(this);
    }

    for(int i = 0; i < m_ghosts.size(); i++)
    {
        m_ghosts[i]->m_start_pos = m_ghost_starts[i];
        m_ghosts[i]->m_target = hero_unit;
        m_ghosts[i]->BeginPlay(this);
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

}

void CoPmLevel::_Render( RenderContext& render_ctxt )
{
#if 0
    u8vec4 col_white = u8vec4(255,255,255,255);
    u8vec4 col_red = u8vec4(255,0,0,255);
    u8vec4 col_green = u8vec4(0,255,0,255);
    DrawUtils::GetStaticInstance()->PushSegment(vec3(0,0,0), vec3(5,0, 0), col_red, col_red);
    DrawUtils::GetStaticInstance()->PushSegment(vec3(0,0,0), vec3(0,5, 0), col_green, col_green);
    DrawUtils::GetStaticInstance()->PushSegment(vec3(0,0,0), vec3(0,0, 5), col_white, col_white);
#endif
    
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    mat4 cam_to_world_mat(render_ctxt.m_view.m_transform.GetRotation(), render_ctxt.m_view.m_transform.GetTranslation(), (float)render_ctxt.m_view.m_transform.GetScale());
    mat4 view_mat = bigball::inverse(cam_to_world_mat);
    mat4 world_mat(quat(1.f), vec3(0.f, 0.f, 0.f), 1.f);
    
    // Render tiles
    Shader* shader = m_tile_shader;

    shader->Bind();
    {
        ShaderUniform uni_world = shader->GetUniformLocation("world_mat");
        shader->SetUniform( uni_world, world_mat );
        ShaderUniform uni_proj = shader->GetUniformLocation("proj_mat");
        shader->SetUniform( uni_proj, render_ctxt.m_proj_mat );
        ShaderUniform uni_view = shader->GetUniformLocation("view_mat");
        shader->SetUniform( uni_view, view_mat );
        ShaderUniform uni_dim_x = shader->GetUniformLocation("tile_dim_x");
        shader->SetUniform( uni_dim_x, m_tile_dim.x );
        
        glBindVertexArray( m_varrays[eVATiles] );
        
        //glBindBuffer(GL_ARRAY_BUFFER, m_vbuffers[eVBShapeMat]);
        //glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)m_shape_matrices.size() * sizeof(mat4), (GLvoid*)m_shape_matrices.Data(), GL_DYNAMIC_DRAW );
        
        //glBindBuffer(GL_ARRAY_BUFFER, m_vbuffers[eVBTileData]);
        //glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)m_shape_params.size() * sizeof(Draw::InstanceParams), (GLvoid*)m_shape_params.Data(), GL_DYNAMIC_DRAW );
        
        //glBindBuffer(GL_ARRAY_BUFFER, m_vbuffers[eVBTileData]);
        //glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)m_tile_draw_instances.size() * sizeof(PmDrawTileInstance), (GLvoid*)m_tile_draw_instances.Data(), GL_STATIC_DRAW );
        
        //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glDrawElementsInstanced( GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0, m_tiles.size() );
        
        glBindVertexArray(0);
    }
    shader->Unbind();

    
    // Render balls
    if (m_need_ball_redraw)
    {
        m_need_ball_redraw = false;
        glBindBuffer(GL_ARRAY_BUFFER, m_vbuffers[eVBTileBall]);
        glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)m_tile_balls.size() * sizeof(PmTileBall), (GLvoid*)m_tile_balls.Data(), GL_STATIC_DRAW );
    }
    
    shader = m_ball_shader;
    shader->Bind();
    {
        ShaderUniform uni_world = shader->GetUniformLocation("world_mat");
        shader->SetUniform( uni_world, world_mat );
        ShaderUniform uni_proj = shader->GetUniformLocation("proj_mat");
        shader->SetUniform( uni_proj, render_ctxt.m_proj_mat );
        ShaderUniform uni_view = shader->GetUniformLocation("view_mat");
        shader->SetUniform( uni_view, view_mat );
        ShaderUniform uni_dim_x = shader->GetUniformLocation("tile_dim_x");
        shader->SetUniform( uni_dim_x, m_tile_dim.x );
        
        glBindVertexArray( m_varrays[eVABalls] );

        glDrawElementsInstanced( GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0, m_tile_balls.size() );
        
        glBindVertexArray(0);
    }
    shader->Unbind();
}

void CoPmLevel::LoadShaders()
{
    m_tile_shader = GfxManager::GetStaticInstance()->LoadShader( "tile" );
    m_ball_shader = GfxManager::GetStaticInstance()->LoadShader( "ball" );
}

void CoPmLevel::CreateBuffers()
{
    glGenVertexArrays( eVACount, m_varrays );
    glGenBuffers( eVBCount, m_vbuffers );
    
    
    //////////////////////////////////////////////////////////
    // tile quad
    const vec2 tile_vertices[] = { vec2(0.f,0.f), vec2(1.f,0.f), vec2(1.f,1.f), vec2(0.f,1.f) };
    
    GLuint idx_data[] = {
        0,2,1, 0,3,2
    };
    
    glBindVertexArray( m_varrays[eVATiles] );
    {
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_vbuffers[eVBTileElt] );
        glBufferData( GL_ELEMENT_ARRAY_BUFFER, COUNT_OF(idx_data) * sizeof(GLuint), idx_data, GL_STATIC_DRAW );
        
        glBindBuffer( GL_ARRAY_BUFFER, m_vbuffers[eVBTile] );
        glBufferData( GL_ARRAY_BUFFER, COUNT_OF(tile_vertices) * sizeof(vec2), tile_vertices, GL_STATIC_DRAW );
        glEnableVertexAttribArray(0);
        glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, sizeof(vec2) /*stride*/, (void*)0 /*offset*/ );
        
        //glBindBuffer( GL_ARRAY_BUFFER, m_vbuffers[eVBShapeMat] );
        //for (int i = 0; i < 4; i++ )
        //{
        //    glEnableVertexAttribArray( 1 + i );
        //    glVertexAttribPointer( 1 + i, 4, GL_FLOAT, GL_FALSE, sizeof(mat4) /*stride*/, (void*)(sizeof(vec4) * i) /*offset*/ );
        //    glVertexAttribDivisor( 1 + i, 1 );
        //}
     

        uintptr_t offset_params = 0;
        glBindBuffer( GL_ARRAY_BUFFER, m_vbuffers[eVBTileWall] );
        glEnableVertexAttribArray(1);
        glVertexAttribPointer( 1, 4, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(PmTile), (void*)offset_params );
        glVertexAttribDivisor( 1, 1 );
        offset_params += sizeof(u8vec4);
        
        //glBindBuffer(GL_ARRAY_BUFFER, m_vbuffers[eVBTileData]);
        //glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)m_tile_draw_instances.size() * sizeof(PmDrawTileInstance), (GLvoid*)m_tile_draw_instances.Data(), GL_STATIC_DRAW );
     
        //glEnableVertexAttribArray(6);
        //glVertexAttribPointer( 6, 4, GL_FLOAT, GL_FALSE, sizeof(Draw::InstanceParams) /*stride*/, (void*)offset_params );
        //glVertexAttribDivisor( 6, 1 );
        //offset_params += sizeof(vec4);
        
        //glEnableVertexAttribArray(7);
        //glVertexAttribPointer( 7, 3, GL_FLOAT, GL_FALSE, sizeof(Draw::InstanceParams) /*stride*/, (void*)offset_params);
        //glVertexAttribDivisor( 7, 1 );
        
        glBindVertexArray(0);
        for( int attrib_idx = 0; attrib_idx < 8; attrib_idx++)
            glDisableVertexAttribArray( attrib_idx);
    }
    
    glBindVertexArray( m_varrays[eVABalls] );
    {
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_vbuffers[eVBTileElt] );
        glBindBuffer( GL_ARRAY_BUFFER, m_vbuffers[eVBTile] );
        glEnableVertexAttribArray(0);
        glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, sizeof(vec2) /*stride*/, (void*)0 /*offset*/ );
        
        uintptr_t offset_params = 0;

        glBindBuffer( GL_ARRAY_BUFFER, m_vbuffers[eVBTileBall] );
        glEnableVertexAttribArray(1);
        glVertexAttribPointer( 1, 4, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(PmTileBall), (void*)offset_params );
        glVertexAttribDivisor( 1, 1 );
        offset_params += sizeof(u8vec4);
        
        glBindVertexArray(0);
        for( int attrib_idx = 0; attrib_idx < 8; attrib_idx++)
            glDisableVertexAttribArray( attrib_idx);
    }

}

void CoPmLevel::DestroyBuffers()
{
    glDeleteBuffers( eVBCount, m_vbuffers );
    glDeleteVertexArrays( eVACount, m_varrays );
}

int32 CoPmLevel::CanViewPosition(ivec2 from, ivec2 to) const
{
    if (to.y == from.y)
    {
        if (to.x == from.x)
            return 1;
            
        int32 a = bigball::min(to.x, from.x);
        int32 b = bigball::max(to.x, from.x);
        for(int32 t = a; t < b; t++)
        {
            if (GetTile(t, from.y).m_right == 0)
                return INDEX_NONE;
        }
        return to.x > from.x ? 1 /*right*/ : 0 /*left*/;
    
    }
    else if (to.x == from.x)
    {
        int32 a = bigball::min(to.y, from.y);
        int32 b = bigball::max(to.y, from.y);
        for(int32 t = a; t < b; t++)
        {
            if (GetTile(from.x, t).m_down == 0)
                return INDEX_NONE;
        }
        return to.y > from.y ? 3 /*down*/ : 2 /*up*/;
    }
    
    return INDEX_NONE;
}

