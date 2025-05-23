#ifndef COPMLEVEL_H
#define COPMLEVEL_H

#include "engine/component.h"
#include "../game/colevel.h"
#include "gfx/shader.h"

namespace bigfx
{
	class BIGFX_API Shader;
	struct BIGFX_API TickContext;
	struct BIGFX_API RenderContext;
};

class CoPmUnit;

struct PmTile
{
    enum class eEdgeType : uint8
    {
        None = 0,
        Portal,
        Wall,
        GhostSink,
    };
    static constexpr int TypeSink = 1;

    union
    {
        struct
        {
            eEdgeType	m_left;
            eEdgeType	m_right;
            eEdgeType	m_up;
            eEdgeType	m_down;
        };
        eEdgeType       m_dir[4];
    };
    int  m_type;
};

struct PmTileBall
{
    union
    {
        struct
        {
            uint8   m_big;
            uint8   m_center;
            uint8   m_right;
            uint8   m_down;
        };
        uint32      m_data;
    };
};

struct PmTileSink
{
    union
    {
        struct
        {
            uint8	m_left;
            uint8	m_right;
            uint8	m_up;
            uint8	m_down;
        };
        uint8       m_dir[4];
        uint32      m_data;
    };
};

enum class ePmGameState : uint32
{
    Run = 0,
    HeroDie,
    GhostFlee
};

//////////////////////////////////////////////////
class CoPmLevel : public CoLevel
{
	CLASS_EQUIP_H(CoPmLevel, CoLevel)

public:
						CoPmLevel();
	virtual				~CoPmLevel();

	static Component*	NewComponent()		{ return new CoPmLevel();	}

	virtual void		Create( Entity* owner, class json::Object* proto = nullptr ) override;
	virtual void		Destroy() override;
	virtual void		AddToWorld() override;
	virtual void		RemoveFromWorld() override;
	virtual void		Tick( TickContext& tick_ctxt ) override;
	virtual void		_Render( RenderContext& render_ctxt ) override;
    virtual void        GetLevelBounds(vec2& bmin, vec2& bmax) const override;
    
    virtual void        BeginPlay(bool new_game) override;
    virtual void        OnControllerInput( Camera* camera, ControllerInput const& input ) override;

    PmTile&             GetTile(int i, int j)       { return m_tiles[m_tile_dim.x * j + i]; }
    PmTile const&       GetTile(int i, int j) const { return m_tiles[m_tile_dim.x * j + i]; }
    PmTileBall&         GetTileBall(int i, int j)   { return m_tile_balls[m_tile_dim.x * j + i]; }
    PmTileSink&         GetTileSink(int i, int j)   { return m_tile_sinks[m_tile_dim.x * j + i]; }
	ivec2               GetTileCoord(vec2 pos, vec2& frac_xy);
    
    void                SetBallEatenAtPos(const vec2& pos)  { m_last_ball_eaten_pos = pos; m_last_ball_eaten = true; }
    void                ResetBallEaten()                    { m_last_ball_eaten = false; }
    int32               CanViewPosition(ivec2 from, ivec2 to) const;
    int32               ReverseDir(int32 dir) const;
    
    void                RequestGameStateChange(ePmGameState new_state) { m_pending_game_state = new_state; m_state_change_request = true; }
    ePmGameState        GetGameState() const        { return m_current_game_state;  }
    float               GetGameStateTimer() const   { return m_state_timer;         }

public:
    ivec2               m_tile_dim;
    ivec2               m_hero_start;
    Array<ivec3>        m_ghost_starts;
    Array<PmTile>       m_tiles;
    Array<PmTileBall>   m_tile_balls;
    // field representing closest sink directions
    Array<PmTileSink>   m_tile_sinks;
    Array<CoPmUnit*>    m_ghosts;
    
    // game state
    ePmGameState        m_current_game_state;
    ePmGameState        m_pending_game_state;
    float               m_state_timer;
    
    // rendering stuff
    bgfx::ProgramHandle         m_tile_shader;
    bgfx::ProgramHandle         m_ball_shader;
    bgfx::UniformHandle         m_u_tile_dim;

    bgfx::VertexBufferHandle	m_vbh_quad;
    bgfx::IndexBufferHandle		m_ibh_quad;

    vec2                m_last_ball_eaten_pos;
    bool                m_last_ball_eaten;
    bool                m_state_change_request;
    
    void                LoadShaders();
    void                CreateBuffers();
    void                DestroyBuffers();
};



#endif // COPMLEVEL_H
