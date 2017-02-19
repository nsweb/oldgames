


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

protected:
	Array<CoLevel*>		m_levels;
    int                 m_current_level_idx;

	//LevelShader*		m_levelshader;
};


#endif // OGWORLD_H
