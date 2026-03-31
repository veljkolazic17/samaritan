#pragma once
#include <defines.hpp>

#ifdef SM_TOOL
#include <objecttemplates/singleton.hpp>
#include <objecttemplates/shutdownable.hpp>

#include <engine/events/eventhandlerwrapper.hpp>
#include <engine/input/events/mouseevents.hpp>

#include <vector>
#include <limits>

#define smEntityPicker() ::samaritan::EntityPicker::GetInstance()

BEGIN_NAMESPACE

class EntityPicker SINGLETON(EntityPicker)
{
private:
    SINGLETON_CONSTRUCTOR(EntityPicker);

public:
    void SingletonInit() override;
    void SingletonShutdown() override;

    SM_INLINE smuint32 GetPickedObjectId() const { return m_PickedObjectId; }

private:
    void HandleOnMouseButtonPressed(const Input::MouseButtonPressedEvent& event);

    Events::EventHandler<Input::MouseButtonPressedEvent> m_MouseButtonPressedHandler;

    smuint32 m_PickedObjectId = UINT_MAX;
    smint16 m_ClickX = 0;
    smint16 m_ClickY = 0;
    smbool m_PickRequested = false;
};


END_NAMESPACE
#endif
