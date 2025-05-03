
#ifndef OGUNITMANAGER_H
#define OGUNITMANAGER_H

#include "engine/componentmanager.h"

namespace bigfx
{
	class BIGFX_API Component;
	class BIGFX_API Shader;
    struct BIGFX_API RenderContext;
};
class CoUnit;

class UnitManager : public ComponentManager
{
	STATIC_MANAGER_H(UnitManager);

private:
	typedef ComponentManager Super;

public:
						UnitManager();
	virtual				~UnitManager();

	virtual void		Create();
	virtual void		Destroy();	
	virtual void		AddComponentToWorld( Component* component );
	virtual void		RemoveComponentFromWorld( Component* component );
	virtual void		Tick( struct TickContext& tick_ctxt );
    virtual void		_Render( bigfx::RenderContext& render_ctxt );

	Array<CoUnit*> const& GetUnitArray()	{ return m_units;	}

protected:
	Array<CoUnit*>		m_units;
};


#endif // OGUNITMANAGER_H
