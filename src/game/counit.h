


#ifndef OGCOUNIT_H
#define OGCOUNIT_H

#include "engine/component.h"


namespace bigball
{
	class BIGBALL_API Shader;
	struct BIGBALL_API TickContext;
	struct BIGBALL_API RenderContext;
    class BIGBALL_API Camera;
    struct ControllerInput;
};

enum class eUnitState
{
    Run = 0,
    Edit,
};

class CoUnit : public Component
{
	CLASS_EQUIP_H(CoUnit, Component)

public:
						CoUnit();
	virtual				~CoUnit();

	static Component*	NewComponent()		{ return new CoUnit();	}

	virtual void		Create( Entity* owner, class json::Object* proto = nullptr ) override;
	virtual void		Destroy() override;
	virtual void		AddToWorld() override;
	virtual void		RemoveFromWorld() override;
	virtual void		Tick( TickContext& tick_ctxt );
	void				_Render( RenderContext& render_ctxt );

	void				ChangeState(eUnitState new_state);
	void				SetCurrentLevel(Entity* current_level);

public:
	Entity*		m_current_level;
	Shader*     m_shader;

    eUnitState  m_state;

    /** Prototype parameters */
    float       m_speed;
    
    enum eVAType
    {
        eVAUnit = 0,
        eVACount
    };
    enum eVBType
    {
        eVBUnit = 0,
        eVBUnitElt,
        eVBCount
    };
    
    // rendering stuff
    GLuint		m_varrays[eVACount];
    GLuint		m_vbuffers[eVBCount];
};

#endif // OGCOUNIT_H
