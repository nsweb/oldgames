


#ifndef COLEVEL_H
#define COLEVEL_H

#include "engine/component.h"
#include "gfx/shader.h"

namespace bigball
{
	class BIGBALL_API Shader;
	struct BIGBALL_API TickContext;
	struct BIGBALL_API RenderContext;
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
	//void				InterpAndSetUniforms( float time );
	virtual void		_Render( RenderContext& render_ctxt);


public:
    Name                            m_level_name;
};



#endif // COLEVEL_H
