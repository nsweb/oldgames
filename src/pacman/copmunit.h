


#ifndef OGCOPMUNIT_H
#define OGCOPMUNIT_H

#include "../game/counit.h"

class CoPmLevel;

class CoPmUnit : public CoUnit
{
	CLASS_EQUIP_H(CoPmUnit, CoUnit)

public:
						CoPmUnit();
	virtual				~CoPmUnit();

	static Component*	NewComponent()		{ return new CoPmUnit();	}

	virtual void		Create( Entity* owner, class json::Object* proto = nullptr ) override;
	virtual void		Destroy() override;
	virtual void		AddToWorld() override;
	virtual void		RemoveFromWorld() override;
	virtual void		Tick(TickContext& tick_ctxt) override;
	void				OnControllerInput( ControllerInput const& input);
    void                BeginPlay(CoPmLevel* level, bool new_game);

public:
    CoPmUnit*   m_target;
    ivec2       m_start_pos;
    ivec2       m_last_tile_coord;
    vec2        m_input_vector;
	vec2        m_move_vector;
    int32       m_hero;
    int32       m_initial_life;
    bool        m_is_weak;
    
    CoPmLevel*  m_current_level;
};

#endif // OGCOPMUNIT_H
