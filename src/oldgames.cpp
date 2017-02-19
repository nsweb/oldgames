

#include "oldgames.h"
#include "system/profiler.h"
#include "SDL_main.h"


#ifdef __cplusplus
extern "C"
#endif
int main(int argc, char *argv[])
{
	PROFILE_THREAD_SCOPE("MainThread");

	g_pEngine = new OGEngine();
	CommandLine& CmdLine = g_pEngine->GetCommandLine();
	CmdLine.Parse( argc, argv );
    
    EngineInitParams init_params;
    init_params.resizable_window = false;
    init_params.mouse_capture = true;
    init_params.default_res_x = 800;
    init_params.default_res_y = 600;
	g_pEngine->Init( init_params );

	String CmdType;
	if( CmdLine.IsCommand( CmdType ) )
	{
		g_pEngine->RunCommand( CmdType, CmdLine.switches, CmdLine.tokens );
	}
	else
	{
		g_pEngine->MainLoop();
	}

	g_pEngine->Shutdown();
	delete g_pEngine;
	g_pEngine = nullptr;

	return 0;
}
