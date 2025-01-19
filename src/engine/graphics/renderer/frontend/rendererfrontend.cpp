#include <engine/graphics/renderer/frontend/rendererfrontend.hpp>
#include <engine/graphics/renderer/backend/vulkan/vulkanbackend.hpp>

#include<engine/memory/memory.hpp>
#include <engine/events/eventmanager.hpp>

#ifdef TEST_CODE_ENABLED
#include <math/matrix.hpp>
#include <math/math.hpp>
#endif

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
			if (m_RendererBackend->BeginFrame(renderData.m_Time))
			{
#ifdef TEST_CODE_ENABLED
				smMat4 projection = smMat4::Perspective(Math::Deg2Rad(45.0f), 1280 / 720.0f, 0.1f, 1000.0f);
				static mfloat32 z = -1.0f;
				z -= 0.005f;
				smMat4 view = smMat4Translation(smVec3{ 0, 0, z });
				m_RendererBackend->UpdateGlobalState(projection, view, smVec3_zero, smVec4_one, 0);
#endif

				if (!m_RendererBackend->EndFrame(renderData.m_Time))
				{
					softAssert(false, "Error ending frame!");
				}
				m_RendererBackend->IncrementFrameCount();
			}			
		}
		else
		{
			hardAssert(false, "Renderer not initialized!");
		}
	}

	void Renderer::Resize(muint32 width, muint32 heigth)
	{
		if (m_RendererBackend)
		{
			m_RendererBackend->Resize(width, heigth);
		}
	}
}

END_NAMESPACE