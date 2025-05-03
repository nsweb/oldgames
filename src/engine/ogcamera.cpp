
#include "../oldgames.h"
#include "ogcamera.h"
#include "engine/coposition.h"
#include "engine/controller.h"
#include "../game/colevel.h"



CLASS_EQUIP_CPP(OGCameraCtrl_2D);
//CLASS_EQUIP_CPP(FSCameraCtrl_EditPath);

OGCameraCtrl_2D::OGCameraCtrl_2D() :
	m_level(nullptr)
{

}

void OGCameraCtrl_2D::UpdateView( CameraView& cam_view, float delta_seconds )
{

    
    Super::UpdateView( cam_view, delta_seconds );
}

bool OGCameraCtrl_2D::OnControllerInput( Camera* camera, ControllerInput const& input )
{
    if( input.m_type == eCIT_Key )
    {
        if (m_level)
        {
            m_level->OnControllerInput(camera, input);
        }
        else
        {
            mat4 cam_to_world( camera->GetRotation() );
            vec3 right = cam_to_world.v0.xyz;
            vec3 up = cam_to_world.v1.xyz;
            vec3 front = -cam_to_world.v2.xyz;
            
            static float strafe_speed = 20.f;
            vec3 cam_pos = camera->GetPosition();
            cam_pos += (right * input.m_delta.x + up * input.m_delta.z + front * input.m_delta.y) * strafe_speed;
            camera->SetPosition(cam_pos);
        }
    }
    
	return Super::OnControllerInput( camera, input );
}

void OGCameraCtrl_2D::OnLevelReset( CoLevel* level )
{
    m_level = level;
    if(!level)
        return;
    
    // center camera
    vec2 bmin, bmax;
    level->GetLevelBounds(bmin, bmax);
    
    Camera* camera = Controller::GetStaticInstance()->GetActiveCamera();
    if(!camera)
        return;
    
    CameraView& view = camera->GetView();
    float fov_h = view.m_parameters[eCP_FOV] * (F_PI / 180.0f);
    float level_width = bmax.x - bmin.x;
    float cam_height = level_width * 0.5f / bigfx::tan(fov_h * 0.5f);
    
    vec3 cam_pos;
    cam_pos.xy = (bmin + bmax) * 0.5f;
    cam_pos.z = -cam_height;
    camera->SetPosition(cam_pos);
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#if 0

FSCameraCtrl_EditPath::FSCameraCtrl_EditPath() :
		m_strafe_speed(1.f),
		m_rotation_speed(1.f),
		//m_ptarget(nullptr),
		m_current_cp_edit(0),
		m_edit_slide(0.f)
{

}

void FSCameraCtrl_EditPath::UpdateView( CameraView& cam_view, float delta_seconds )
{

    float knot_dist = m_ptarget->GetSumKnotDistance( m_current_cp_edit );
    knot_dist += m_edit_slide;
    
    transform tf;
    m_ptarget->InterpPathKnotDist( knot_dist, tf );
	m_interp_pos = tf.GetTranslation();
	m_interp_tan = tf.TransformVector( vec3(0.f,0.f,-1.f) );
	

    cam_view.m_transform.SetTranslation( tf.GetTranslation() );
    cam_view.m_transform.SetRotation( tf.GetRotation() );

	Super::UpdateView( cam_view, delta_seconds );
}

bool FSCameraCtrl_EditPath::OnControllerInput( Camera* pcamera, ControllerInput const& input )
{
    CameraView& cam_view = pcamera->GetView();
    
    if( input.m_type == eCIT_Key )
    {
        mat4 cam_to_world( cam_view.m_transform.GetRotation() );
        vec3 right = cam_to_world.v0.xyz;
        vec3 up = cam_to_world.v1.xyz;
        vec3 front = -cam_to_world.v2.xyz;
        
        vec3& cp_pos = m_ptarget->m_ctrl_points[m_current_cp_edit].m_pos;
        
        cp_pos += (right * input.m_delta.x + up * input.m_delta.z + front * input.m_delta.y) * m_strafe_speed;
        m_ptarget->OnControlPointMoved( m_current_cp_edit );
    }
    else if( input.m_type == eCIT_Mouse )
    {
        // TODO : arcball rotation
        //quat YawPitchRoll( quat::fromeuler_xyz( Input.m_Delta.y * m_RotationSpeed, Input.m_Delta.x * m_RotationSpeed, 0.0f ) );
        //View.m_Transform.SetRotation( View.m_Transform.GetRotation() * YawPitchRoll );
    }
    
	return Super::OnControllerInput( pcamera, input );
}

int FSCameraCtrl_EditPath::ResetEdit( float knot_dist_along_path )
{
    int cp_idx = m_ptarget->GetNearestControlPointIdx( knot_dist_along_path );
    m_current_cp_edit = cp_idx;
    m_edit_slide = knot_dist_along_path - m_ptarget->GetSumKnotDistance( cp_idx );
    
    return cp_idx;
}

void FSCameraCtrl_EditPath::BuildGui()
{
    float kdist_before = m_ptarget->GetSumKnotDistance(m_current_cp_edit) - m_ptarget->GetSumKnotDistance(m_current_cp_edit-1) - 1e-2f;
    float kdist_after = m_ptarget->GetSumKnotDistance(m_current_cp_edit+1) - m_ptarget->GetSumKnotDistance(m_current_cp_edit) - 1e-2f;
	ImGui::SliderFloat("slide", &m_edit_slide, -kdist_before, kdist_after);

	if( ImGui::InputFloat3("cp_pos", (float*)&m_ptarget->m_ctrl_points[m_current_cp_edit].m_pos) )
    {
        m_ptarget->OnControlPointMoved( m_current_cp_edit );
    }
	ImGui::InputFloat("knot", (float*)&m_ptarget->m_ctrl_points[m_current_cp_edit].m_knot);

	ImGui::InputFloat3("cam_pos", (float*)&m_interp_pos, -1, ImGuiInputTextFlags_ReadOnly);
	ImGui::InputFloat3("cam_dir", (float*)&m_interp_tan, -1, ImGuiInputTextFlags_ReadOnly);

	m_dist_before = m_ptarget->GetSumDistance(m_current_cp_edit) - m_ptarget->GetSumDistance(m_current_cp_edit-1);
	m_dist_after = m_ptarget->GetSumDistance(m_current_cp_edit+1) - m_ptarget->GetSumDistance(m_current_cp_edit);
	ImGui::InputFloat("dist_before", (float*)&m_dist_before, -1, ImGuiInputTextFlags_ReadOnly);
	ImGui::InputFloat("dist_after", (float*)&m_dist_after, -1, ImGuiInputTextFlags_ReadOnly);

}
#endif // 0
