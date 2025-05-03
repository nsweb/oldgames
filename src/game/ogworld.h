


#ifndef OGWORLD_H
#define OGWORLD_H

#include "engine/componentmanager.h"

namespace bigfx
{
	class BIGFX_API Component;
	class BIGFX_API Shader;
    struct BIGFX_API RenderContext;
};
class CoLevel;

enum class eTransitionType : int32
{
    None = -1,
    PlayerDies = 0,
};

struct OGGameState
{
    int32           m_player_life;
    int32           m_player_points;
    eTransitionType m_transition_type;
    float           m_transition_time;
};

class OGWorld : public ComponentManager
{
    STATIC_MANAGER_H(OGWorld);
    
private:
    typedef ComponentManager Super;

public:
                            OGWorld();
	virtual                 ~OGWorld();

	virtual void            Create();
	virtual void            Destroy();
	virtual void            AddComponentToWorld( Component* pComponent );
	virtual void            RemoveComponentFromWorld( Component* pComponent );
	virtual void            Tick( struct TickContext& TickCtxt );
    virtual void            _Render( bigfx::RenderContext& RenderCtxt );

    bool                    InitLevels( char const* json_path );
	Array<CoLevel*> const&  GetLevelArray()	{ return m_levels;	}
    CoLevel*                GetCurrentLevel();
    bool                    SetCurrentLevel( int level_idx );
    void                    RequestTransition( eTransitionType type );

protected:
	Array<CoLevel*>		m_levels;
    int                 m_current_level_idx;
    OGGameState         m_game_state;
    
    // rendering stuff
    bgfx::ProgramHandle         m_transition_shader;
    bgfx::UniformHandle         m_u_transition;
    bgfx::VertexBufferHandle	m_vbh_quad;
    bgfx::IndexBufferHandle		m_ibh_quad;
    
    void                LoadShaders();
    void                CreateBuffers();
    void                DestroyBuffers();
};


#endif // OGWORLD_H
