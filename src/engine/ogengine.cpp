// ogengine.cpp
//

#include "../oldgames.h"
#include "engine/entity.h"
#include "engine/entitymanager.h"
#include "engine/controller.h"
#include "engine/coposition.h"
//#include "ui/uimanager.h"
#include "../game/counit.h"
#include "../game/unitmanager.h"
#include "../game/colevel.h"
#include "../game/ogworld.h"
#include "../pacman/copmlevel.h"
//#include "../engine/dfmanager.h"
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
	//m_peditor = new FSEditor();
	//m_peditor->Init();


	//////////////////////////////////////////////////////////////////////////
	// Scene description
    //FSWorld::GetStaticInstance()->InitLevels( "../data/world.json" );
    
	//Entity* pship = EntityManager::GetStaticInstance()->CreateEntityFromJson( "../data/ship.json", "Ship" );
	//EntityManager::GetStaticInstance()->AddEntityToWorld( pship );

	Entity* pac_level = EntityManager::GetStaticInstance()->CreateEntityFromJson( "../data/pacman/level.json", "Level_0" );
	EntityManager::GetStaticInstance()->AddEntityToWorld( pac_level );

	Entity* pac_man = EntityManager::GetStaticInstance()->CreateEntityFromJson("../data/pacman/pacman.json", "Pacman_0");
	EntityManager::GetStaticInstance()->AddEntityToWorld(pac_man);

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
	}


	// Link scene objects
	CameraCtrl_Base* cam_ctrl = Controller::GetStaticInstance()->GetCameraCtrl( OGCameraCtrl_2D::StaticClass() );
	//if( cam_ctrl && cam_ctrl->IsA( FSCameraCtrl_Fly::StaticClass() ) )
	//	((FSCameraCtrl_Fly*)cam_ctrl)->SetTarget( pship );

    CoLevel* level = OGWorld::GetStaticInstance()->GetCurrentLevel();
	//CoShip* pcoship = static_cast<CoShip*>( pship->GetComponent( CoShip::StaticClass() ) );
	//if( pcoship )
	//	pcoship->SetCurrentLevel( level->GetEntity() );

	return bInit;
}

void OGEngine::Shutdown()
{
	//m_peditor->Shutdown();
	//BB_DELETE( m_peditor );

	Engine::Shutdown();
}

void OGEngine::MainLoop()
{
	Engine::MainLoop();
}

void OGEngine::DeclareComponentsAndEntities()
{
	Super::DeclareComponentsAndEntities();

	//DECLARE_COMPONENT_MGR( CoPath, PathManager );
	DECLARE_COMPONENT_MGR( CoUnit, UnitManager );
    DECLARE_COMPONENT_MGR( CoPmLevel, OGWorld );
	//DECLARE_COMPONENT( CoPath );
	//DECLARE_ENTITYPATTERN( Ship, Entity, (2, "CoPosition", "CoShip"), (0) );
	DECLARE_ENTITYPATTERN( PacLevel, Entity, (2, "CoPosition", "CoPmLevel"), (0) );
	DECLARE_ENTITYPATTERN( PacUnit, Entity, (2, "CoPosition", "CoUnit"), (0));
}

void OGEngine::CreateGameCameras()
{
	Controller* pController = Controller::GetStaticInstance();
	pController->RegisterCameraCtrl( new OGCameraCtrl_2D() );
	//pController->RegisterCameraCtrl( new FSCameraCtrl_EditPath() );
	pController->SetActiveCameraCtrl( OGCameraCtrl_2D::StaticClass() );
}

void OGEngine::InitManagers()
{
	Super::InitManagers();

    OGWorld* pworld = new OGWorld();
    pworld->Create();
    m_managers.push_back( pworld );

	//PathManager* ppath_manager = new PathManager();
	//ppath_manager->Create();
	//m_managers.push_back( ppath_manager );

    /*DFManager* pdf_manager = new DFManager();
    pdf_manager->Create();
    m_managers.push_back( pdf_manager );
    
    ShipManager* pship_manager = new ShipManager();
    pship_manager->Create();
    m_managers.push_back( pship_manager );*/
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
