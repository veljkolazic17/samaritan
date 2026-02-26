#pragma once
#include <defines.hpp>
#if IMGUI_DISPLAY_ENABLED

#include "imgui.h"

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

    void DrawMenuBar();
    void DrawOpenModules();

private:
    void DrawMenuNode(ImguiMenuNode& node);
    void DrawOpenNodes(std::vector<ImguiMenuNode>& nodes);

    std::vector<ImguiMenuNode> m_RootNodes;
};

END_NAMESPACE
#endif
