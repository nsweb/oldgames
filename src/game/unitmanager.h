


#ifndef OGUNITMANAGER_H
#define OGUNITMANAGER_H

#include "engine/componentmanager.h"

namespace bigball
{
	class BIGBALL_API Component;
	class BIGBALL_API Shader;
    struct BIGBALL_API RenderContext;
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
    virtual void		_Render( bigball::RenderContext& render_ctxt );

	Array<CoUnit*> const& GetUnitArray()	{ return m_units;	}

protected:
	Array<CoUnit*>		m_units;
};


#endif // OGUNITMANAGER_H
