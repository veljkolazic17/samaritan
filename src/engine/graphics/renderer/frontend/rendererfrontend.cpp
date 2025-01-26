#include <engine/graphics/renderer/frontend/rendererfrontend.hpp>
#include <engine/graphics/renderer/backend/vulkan/vulkanbackend.hpp>

#include<engine/memory/memory.hpp>
#include <engine/events/eventmanager.hpp>

#ifdef TEST_CODE_ENABLED
#include <math/matrix.hpp>
#include <math/math.hpp>
#include <math/vector.hpp>
#endif

BEGIN_NAMESPACE

namespace Graphics
{
	void Renderer::Init()
	{
		if (m_RendererBackend)
		{
			m_NearClip = 0.1f;
			m_FarClip = 1000.0f;
			m_Projection = smMat4::Perspective(Math::Deg2Rad(45.0f), 1280 / 720.0f, 0.1f, 1000.0f);
			m_View = smMat4Translation(smVec3{ 0, 0, 30.0f });
			m_View.InverseFastSelf();

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
				m_RendererBackend->UpdateGlobalState(m_Projection, m_View, smVec3_zero, smVec4_one, 0);
				m_RendererBackend->UpdateObject(smMat4_zero);
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
		if (m_RendererBackend and heigth != 0 and width != 0)
		{
			m_Projection = smMat4::Perspective(Math::Deg2Rad(45.0f), width / heigth, m_NearClip, m_FarClip);

			m_RendererBackend->Resize(width, heigth);
		}
	}
}

END_NAMESPACE