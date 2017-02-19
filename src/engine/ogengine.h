
// ogengine.h


#ifndef OGENGINE_H
#define OGENGINE_H

#include "engine/engine.h"

class OGEngine : public Engine
{
private:
	typedef Engine Super;

public:
						OGEngine();
	virtual				~OGEngine();

	virtual bool		Init(EngineInitParams const& init_params) override;
	virtual void		Shutdown() override;
	virtual void		MainLoop() override;
	virtual bool		RunCommand( String const& cmd_type, Array<String> const& switches, Array<String> const& tokens ) override;

	static OGEngine*	Get()		{ return ms_pengine;	}

protected:
	virtual void		DeclareComponentsAndEntities() override;
	virtual void		CreateGameCameras() override;
	virtual void		InitManagers() override;
	virtual void		DestroyManagers() override;

	static OGEngine*	ms_pengine;
	//class FSEditor*		m_peditor;
};


#endif  // OGENGINE_H
