
#ifndef OGCOUNIT_H
#define OGCOUNIT_H

#include "engine/component.h"


namespace bigfx
{
	class BIGFX_API Shader;
	struct BIGFX_API TickContext;
	struct BIGFX_API RenderContext;
    class BIGFX_API Camera;
    struct ControllerInput;
};

enum class eUnitState
{
    Run = 0,
	Dead,
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
    vec2                GetScreenPos(mat4 const& view_proj);

public:
	Entity*		m_current_level;
    bgfx::ProgramHandle m_unit_program;

    eUnitState  m_state;
    vec4        m_shader_param[2];
    bgfx::UniformHandle m_u_shader_param[2];

    bgfx::VertexBufferHandle	m_vbh_quad;
    bgfx::IndexBufferHandle		m_ibh_quad;

    /** Prototype parameters */
    float       m_speed;
};

#endif // OGCOUNIT_H
