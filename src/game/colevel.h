


#ifndef COLEVEL_H
#define COLEVEL_H

#include "engine/component.h"
#include "gfx/shader.h"

namespace bigball
{
	class BIGBALL_API Shader;
    class BIGBALL_API Camera;
	struct BIGBALL_API TickContext;
	struct BIGBALL_API RenderContext;
    struct ControllerInput;
};

//////////////////////////////////////////////////
class CoLevel : public Component 
{
	CLASS_EQUIP_H(CoLevel, Component)

public:
						CoLevel();
	virtual				~CoLevel();

	static Component*	NewComponent()		{ return new CoLevel();	}

	virtual void		Create( Entity* owner, class json::Object* proto = nullptr ) override;
	virtual void		Destroy() override;
	virtual void		AddToWorld() override;
	virtual void		RemoveFromWorld() override;
	virtual void		Tick( TickContext& tick_ctxt );

	virtual void		_Render( RenderContext& render_ctxt);
    virtual void        GetLevelBounds(vec2& bmin, vec2& bmax) const    { bmin = vec2(0.f,0.f); bmax = vec2(1.f,1.f); }
    
    virtual void        BeginPlay() {}
    virtual void        OnControllerInput( Camera* camera, ControllerInput const& input )   {}
    void                SetHero( Entity* hero )                         { m_hero = hero;                              }
    

public:
    Name                            m_level_name;
    Name                            m_game_name;
    Entity*                         m_hero;
};



#endif // COLEVEL_H
