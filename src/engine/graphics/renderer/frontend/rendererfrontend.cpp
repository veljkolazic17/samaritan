#include <engine/graphics/renderer/frontend/rendererfrontend.hpp>
#include <engine/graphics/renderer/backend/vulkan/vulkanbackend.hpp>

#include<engine/memory/memory.hpp>

BEGIN_NAMESPACE

namespace Graphics
{
	void Renderer::Init()
	{
		if (m_RendererBackend)
		{
			m_RendererBackend->Init();
		}
		else
		{
			hardAssert(false, "Renderer not initialized!");
		}
	}

	void Renderer::Init(RendererType rendererType)
	{
		if (rendererType != RendererType::Vulkan && rendererType != RendererType::DirectX12)
		{
			hardAssert(false, "Specified renderer is not supported!");
		}
		switch (rendererType)
		{
			case samaritan::Graphics::RendererType::Vulkan:
			{
				m_RendererBackend = Memory::mnew<VulkanRenderer>(Memory::MemoryTag::MEM_RENDERER);
			}
			break;
			case samaritan::Graphics::RendererType::DirectX12:
			{

			}
			break;
		}
		Init();
	}

	void Renderer::Shutdown()
	{
		if (m_RendererBackend)
		{
			m_RendererBackend->Shutdown();
		}
		
		gpFreeRenderer(m_RendererBackend, sizeof(m_RendererBackend));
	}

	void Renderer::DrawFrame(RenderData& renderData)
	{
		if (m_RendererBackend)
		{
			m_RendererBackend->BeginFrame(renderData.m_Time);

			// here

			m_RendererBackend->EndFrame(renderData.m_Time);
			m_RendererBackend->IncrementFrameCount();
		}
		else
		{
			hardAssert(false, "Renderer not initialized!");
		}
	}
}

END_NAMESPACE