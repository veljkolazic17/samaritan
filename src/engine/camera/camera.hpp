#pragma once
#include <defines.hpp>

#include<math/matrix.hpp>
#include <objecttemplates/singleton.hpp>

#define smCamera()   ::samaritan::Camera::GetInstance()

BEGIN_NAMESPACE

class Camera SINGLETON(Camera)
{
public:
	SINGLETON_CONSTRUCTOR(Camera);
    
    void ChangePitch(smfloat32 ammount);
    void ChangeYaw(smfloat32 ammount);
    void ChangeRoll(smfloat32 ammount);
    void CalculateNewView();

    void SingletonInit() override;

    SM_INLINE smVec3& GetCameraPosition() { return m_CameraPosition; }
    SM_INLINE smMat4& GetCameraView() { return m_View; }
    SM_INLINE void SetIsViewDirty(smbool isViewDirty) { m_IsViewDirty = isViewDirty; }

private:
    smMat4 m_View;
    smVec3 m_CameraPosition;
    smVec3 m_CameraEuler;
    smbool m_IsViewDirty;
};

SM_INLINE void Camera::CalculateNewView()
{
    if (m_IsViewDirty)
    {
        smMat4 rotation = smMat4::RotationEuler(m_CameraEuler.m_X, m_CameraEuler.m_Y, m_CameraEuler.m_Z);
        smMat4 translation = smMat4Translation(m_CameraPosition);
        m_View = rotation * translation;
        m_View.InverseFastSelf();
        m_IsViewDirty = false;
    }
}

SM_INLINE void Camera::ChangePitch(smfloat32 ammount)
{
    m_CameraEuler.m_X += ammount;
    smfloat32 limit = Math::Deg2Rad(89.0f);
    m_CameraEuler.m_X = Math::ClampFloat(-limit, limit, m_CameraEuler.m_X);
    m_IsViewDirty = true;
}

SM_INLINE void Camera::ChangeYaw(smfloat32 ammount)
{
    m_CameraEuler.m_Y += ammount;
    m_IsViewDirty = true;
}

SM_INLINE void Camera::ChangeRoll(smfloat32 ammount)
{
    m_CameraEuler.m_Z += ammount;
    m_IsViewDirty = true;
}

SM_INLINE void Camera::SingletonInit()
{
    m_CameraPosition = smVec3{ 0, 0, 30.0f };
    m_CameraEuler = smVec3_zero;
}

END_NAMESPACE