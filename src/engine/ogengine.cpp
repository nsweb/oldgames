// ogengine.cpp
//

#include "../oldgames.h"
#include "engine/entity.h"
#include "engine/entitymanager.h"
#include "engine/controller.h"
#include "engine/coposition.h"
//#include "ui/uimanager.h"
#include "../game/unitmanager.h"
#include "../game/colevel.h"
#include "../game/ogworld.h"
#include "../pacman/copmlevel.h"
#include "../pacman/copmunit.h"
#include "../engine/ogcamera.h"

OGEngine* OGEngine::ms_pengine = nullptr;

OGEngine::OGEngine()
{
	ms_pengine = this;
}

OGEngine::~OGEngine()
{
	ms_pengine = nullptr;
}

bool OGEngine::Init(EngineInitParams const& init_params)
{
	bool bInit = Engine::Init(init_params);
    
	//////////////////////////////////////////////////////////////////////////
	// Scene description
    OGWorld::GetStaticInstance()->InitLevels( "../data/world.json" );
	
	Entity* ent_camera = EntityManager::GetStaticInstance()->CreateEntityFromJson( "../data/defaultcamera.json" );
	EntityManager::GetStaticInstance()->AddEntityToWorld( ent_camera );
	if( ent_camera->IsA( Camera::StaticClass() ) )
	{
		float cam_dist = 10.0;
		Camera* camera = static_cast<Camera*>( ent_camera );
		camera->SetPosition( dvec3(0.f, 0.f, -cam_dist) );
        mat3 cam_rot;
        // Back
        cam_rot.v2 = vec3(0.f, 0.f, -1.f);
        // Right
        cam_rot.v0 = vec3(1.f, 0.f, 0.f);
        // Up
        cam_rot.v1 = vec3(0.f, -1.f, 0.f);
        camera->SetRotation( quat(cam_rot) );
        
        Controller::GetStaticInstance()->SetActiveCamera(camera);
	}


	// Link scene objects
	CoLevel* level = OGWorld::GetStaticInstance()->GetCurrentLevel();
    level->BeginPlay(true /*new_game*/);
    
    CameraCtrl_Base* cam_ctrl = Controller::GetStaticInstance()->GetCameraCtrl( OGCameraCtrl_2D::StaticClass() );
	if( cam_ctrl && cam_ctrl->IsA( OGCameraCtrl_2D::StaticClass() ) )
		((OGCameraCtrl_2D*)cam_ctrl)->OnLevelReset(level);

	return bInit;
}

void OGEngine::Shutdown()
{
	Engine::Shutdown();
}

bool OGEngine::MainLoop()
{
	return Engine::MainLoop();
}

void OGEngine::DeclareComponentsAndEntities()
{
	Super::DeclareComponentsAndEntities();

	DECLARE_COMPONENT_MGR(CoPmUnit, UnitManager);
	DECLARE_COMPONENT_MGR( CoUnit, UnitManager );
    DECLARE_COMPONENT_MGR( CoPmLevel, OGWorld );
	DECLARE_ENTITYPATTERN( PacLevel, Entity, (2, "CoPosition", "CoPmLevel"), (0) );
	DECLARE_ENTITYPATTERN( PacUnit, Entity, (2, "CoPosition", "CoPmUnit"), (0));
}

void OGEngine::CreateGameCameras()
{
	Controller* pController = Controller::GetStaticInstance();
	pController->RegisterCameraCtrl( new OGCameraCtrl_2D() );
	pController->SetActiveCameraCtrl( OGCameraCtrl_2D::StaticClass() );
}

void OGEngine::InitManagers()
{
	Super::InitManagers();

    OGWorld* pworld = new OGWorld();
    pworld->Create();
    m_managers.push_back( pworld );
    
    UnitManager* unit_manager = new UnitManager();
    unit_manager->Create();
    m_managers.push_back( unit_manager );
}

void OGEngine::DestroyManagers()
{
	Super::DestroyManagers();
}

bool OGEngine::RunCommand( String const& cmd_type, Array<String> const& switches, Array<String> const& tokens )
{
#if 0
	if( cmd_type == "builddata" )
	{
		CmdBuildData cmd;
		return cmd.Run( switches, tokens );
	}
#endif

	return Engine::RunCommand( cmd_type, switches, tokens );
}
