
#include "oldgames.h"
#include "common.h"

namespace
{
	class OldGamesApp : public entry::AppI
	{
	public:
		OldGamesApp(const char* _name, const char* _description, const char* _url)
			: entry::AppI(_name, _description, _url)
		{
		}

		void init(int32_t _argc, const char* const* _argv, uint32_t _width, uint32_t _height) override
		{
			OGEngine* engine = new OGEngine();
			// (REBIND)
			//CommandLine& CmdLine = g_pEngine->GetCommandLine();
			//CmdLine.Parse(_argc, _argv);

			EngineInitParams init_params;
			init_params.resizable_window = false;
			init_params.mouse_capture = true;
			init_params.default_res_x = 1280;
			init_params.default_res_y = 720;
			init_params.argc = _argc;
			init_params.argv = _argv;
			init_params.app = this;
			OGEngine::Get()->Init(init_params);

			// (REBIND)
			//String CmdType;
			//if (CmdLine.IsCommand(CmdType))
			//{
			//	g_pEngine->RunCommand(CmdType, CmdLine.switches, CmdLine.tokens);
			//}
			//else
			//{
			//	g_pEngine->MainLoop();
			//}
		}

		int shutdown() override
		{
			OGEngine::Get()->Shutdown();
			delete OGEngine::Get();

			return 0;
		}

		bool update() override
		{
			return OGEngine::Get()->MainLoop();
		}
	};

} // namespace

ENTRY_IMPLEMENT_MAIN(
	OldGamesApp
	, "_oldgames_"
	, "OldGames game"
	, "https://github.com/nsweb/oldgames"
);

