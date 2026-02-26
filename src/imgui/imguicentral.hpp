#pragma once
#include <defines.hpp>
#if IMGUI_DISPLAY_ENABLED

#include "imgui.h"

#include <engine/events/eventhandlerwrapper.hpp>
#include <engine/input/events/keyboardevents.hpp>
#include <objecttemplates/singleton.hpp>

#define smImguiCentral() ::samaritan::ImguiCentral::GetInstance()

BEGIN_NAMESPACE

//Should This be in every object that wants to use imgui?
class IImguiModule
{
public:
    virtual ~IImguiModule() = default;
    virtual void DrawImgui() = 0;
};

struct ImguiMenuNode
{
    smstring m_Name;
    IImguiModule* m_Module = nullptr;
    smbool m_IsOpen = false;
    std::vector<ImguiMenuNode> m_Children;
};

class ImguiCentral SINGLETON(ImguiCentral)
{
public:
    SINGLETON_CONSTRUCTOR(ImguiCentral);

    void RegisterImguiModule(const smstring& path, IImguiModule* module);

    void DrawImgui();
    
    void SingletonInit() override;
    void SingletonShutdown() override;

private:
    void DrawMenuBar();
    void DrawOpenModules();

    void DrawMenuNode(ImguiMenuNode& node);
    void DrawOpenNodes(std::vector<ImguiMenuNode>& nodes);

    smbool m_IsImguiCentralActive = false;

    void HandleOnKeyboardInputPressedEvent(const Input::KeyboardInputPressedEvent& event);
	Events::EventHandler<Input::KeyboardInputPressedEvent> m_KeyboardInputPressedEventHandler;

    std::vector<ImguiMenuNode> m_RootNodes;
};

END_NAMESPACE
#endif
