


#ifndef OGWORLD_H
#define OGWORLD_H

#include "engine/componentmanager.h"

namespace bigball
{
	class BIGBALL_API Component;
	class BIGBALL_API Shader;
    struct BIGBALL_API RenderContext;
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
    virtual void            _Render( bigball::RenderContext& RenderCtxt );

    bool                    InitLevels( char const* json_path );
	Array<CoLevel*> const&  GetLevelArray()	{ return m_levels;	}
    CoLevel*                GetCurrentLevel();
    bool                    SetCurrentLevel( int level_idx );
    void                    RequestTransition( eTransitionType type );

protected:
	Array<CoLevel*>		m_levels;
    int                 m_current_level_idx;
    OGGameState         m_game_state;

    
    enum eVAType
    {
        eVATransition = 0,
        eVACount
    };
    enum eVBType
    {
        eVBTransition = 0,
        eVBTransitionElt,
        eVBCount
    };
    
    // rendering stuff
    GLuint                  m_varrays[eVACount];
    GLuint                  m_vbuffers[eVBCount];
    Shader*                 m_transition_shader;
    
    void                LoadShaders();
    void                CreateBuffers();
    void                DestroyBuffers();
};


#endif // OGWORLD_H
