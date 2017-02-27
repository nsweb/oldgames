

#ifndef OGCAMERA_H
#define OGCAMERA_H

#include "engine/camera.h"

class CoLevel;

//////////////////////////////////////////////////////////////////////////

class OGCameraCtrl_2D : public CameraCtrl_Base
{
	CLASS_EQUIP_H(OGCameraCtrl_2D, CameraCtrl_Base)

public:
					OGCameraCtrl_2D();

	//float			m_StrafeSpeed;
	//float			m_RotationSpeed;

public:
	virtual void		UpdateView( CameraView& cam_view, float delta_seconds ) override;
	virtual bool		OnControllerInput( Camera* pcamera, struct ControllerInput const& input ) override;
    void				OnLevelReset( CoLevel* level );

protected:
	CoLevel*			m_level;
};

#if 0
class BIGBALL_API FSCameraCtrl_EditPath : public CameraCtrl_Base
{
	CLASS_EQUIP_H(FSCameraCtrl_EditPath, CameraCtrl_Base)

public:
	FSCameraCtrl_EditPath();

	virtual void		UpdateView( CameraView& cam_view, float delta_seconds ) override;
	virtual bool		OnControllerInput( Camera* pcamera, struct ControllerInput const& input ) override;
	//void				SetTarget( CoPath* ptarget )		{ m_ptarget = ptarget; }
	//void				ResetEdit( int at_cp_idx );
	//void				ResetEdit( float nearest_dist_level );
    int                 ResetEdit( float knot_dist_along_path );
	void				BuildGui();

public:
	float			m_strafe_speed;
	float			m_rotation_speed;
	//CoPath*			m_ptarget;

    //ControlPoint    m_backup_cp;
	int             m_current_cp_edit;
	float           m_edit_slide;
	vec3			m_interp_pos;
	vec3			m_interp_tan;
	float			m_dist_before;
	float			m_dist_after;
};
#endif // 0

#endif // OGCAMERA_H
