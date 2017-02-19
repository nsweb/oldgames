


#ifndef COPMLEVEL_H
#define COPMLEVEL_H

#include "engine/component.h"
#include "../game/colevel.h"
#include "gfx/shader.h"

namespace bigball
{
	class BIGBALL_API Shader;
	struct BIGBALL_API TickContext;
	struct BIGBALL_API RenderContext;
};

struct PmTile
{
public:
    uint32	m_left  : 1;
    uint32	m_right : 1;
    uint32	m_up    : 1;
    uint32	m_down  : 1;
    uint32  m_val   : 4;
};

struct PmDrawTileWall
{
    uint8   m_left;
    uint8   m_right;
    uint8   m_up;
    uint8   m_down;
};
struct PmDrawTileBall
{
    uint8   m_big;
    uint8   m_center;
    uint8   m_right;
    uint8   m_down;
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
	virtual void		_Render( RenderContext& render_ctxt );

    PmTile&             GetTile(int i, int j)   { return m_tiles[m_tile_dim.x * j + i]; }


public:
    ivec2               m_tile_dim;
    Array<PmTile>       m_tiles;
    
    enum eVAType
    {
        eVATiles = 0,
        eVABalls,
        eVACount
    };
    enum eVBType
    {
        eVBTile = 0,
        eVBTileWall,
        eVBTileBall,
        eVBTileElt,
        eVBCount
    };
    
    // rendering stuff
    Array<PmDrawTileWall>   m_tile_draw_walls;
    Array<PmDrawTileBall>   m_tile_draw_balls;
    GLuint			m_varrays[eVACount];
    GLuint			m_vbuffers[eVBCount];
    Shader*         m_tile_shader;
    Shader*         m_ball_shader;
    
    
    void                LoadShaders();
    void                CreateBuffers();
    void                DestroyBuffers();
};



#endif // COPMLEVEL_H
