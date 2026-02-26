#include <defines.hpp>
#if IMGUI_DISPLAY_ENABLED
#include <imgui/imguicentral.hpp>

#include <engine/events/eventmanager.hpp>

#include <sstream>

BEGIN_NAMESPACE

static std::vector<smstring> SplitPath(const smstring& path)
{
    std::vector<smstring> parts;
    std::stringstream ss(path);
    smstring part;
    while (std::getline(ss, part, '/'))
    {
        if (!part.empty())
        {
            parts.push_back(part);
        }
    }
    return parts;
}

void ImguiCentral::SingletonInit()
{
    m_KeyboardInputPressedEventHandler = [this](const Input::KeyboardInputPressedEvent& event) { HandleOnKeyboardInputPressedEvent(event); };
	Events::Subscribe<Input::KeyboardInputPressedEvent>(m_KeyboardInputPressedEventHandler);
}

void ImguiCentral::SingletonShutdown()
{
    Events::Unsubscribe<Input::KeyboardInputPressedEvent>(m_KeyboardInputPressedEventHandler);
}

void ImguiCentral::HandleOnKeyboardInputPressedEvent(const Input::KeyboardInputPressedEvent& event)
{
    if(event.m_Key == Input::Key::KEY_F9)
    {
        m_IsImguiCentralActive = !m_IsImguiCentralActive;
    }
}

void ImguiCentral::RegisterImguiModule(const smstring& path, IImguiModule* module)
{
    auto parts = SplitPath(path);
    if (parts.empty())
    {
        return;
    }

    std::vector<ImguiMenuNode>* current = &m_RootNodes;
    for (size_t i = 0; i < parts.size(); ++i)
    {
        const smstring& part = parts[i];

        ImguiMenuNode* found = nullptr;
        for (auto& node : *current)
        {
            if (node.m_Name == part)
            {
                found = &node;
                break;
            }
        }

        if (!found)
        {
            current->emplace_back();
            found = &current->back();
            found->m_Name = part;
        }

        if (i == parts.size() - 1)
        {
            found->m_Module = module;
        }
        else
            current = &found->m_Children;
    }
}

void ImguiCentral::DrawImgui()
{
    if(m_IsImguiCentralActive)
    {
        if (ImGui::BeginMainMenuBar())
        {
            DrawMenuBar();
            ImGui::EndMainMenuBar();
        }
        DrawOpenModules();
    }
}

void ImguiCentral::DrawMenuBar()
{
    for (auto& node : m_RootNodes)
    {
        DrawMenuNode(node);
    }
}

void ImguiCentral::DrawOpenModules()
{
    DrawOpenNodes(m_RootNodes);
}

void ImguiCentral::DrawMenuNode(ImguiMenuNode& node)
{
    if (node.m_Module != nullptr)
    {
        if (ImGui::MenuItem(node.m_Name.c_str(), nullptr, node.m_IsOpen))
        {
            node.m_IsOpen = !node.m_IsOpen;
        }
    }
    else
    {
        if (ImGui::BeginMenu(node.m_Name.c_str()))
        {
            for (auto& child : node.m_Children)
            {
                DrawMenuNode(child);
            }
            ImGui::EndMenu();
        }
    }
}

void ImguiCentral::DrawOpenNodes(std::vector<ImguiMenuNode>& nodes)
{
    for (auto& node : nodes)
    {
        if (node.m_Module != nullptr && node.m_IsOpen)
        {
            if(ImGui::Begin(node.m_Name.c_str(), &node.m_IsOpen))
            {
                node.m_Module->DrawImgui();
            }
            ImGui::End();
        }
        DrawOpenNodes(node.m_Children);
    }
}

END_NAMESPACE
#endif
