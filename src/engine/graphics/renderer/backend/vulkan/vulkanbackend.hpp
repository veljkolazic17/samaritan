#pragma once
#include <defines.hpp>
#include <engine/graphics/renderer/backend/vulkan/vulkantypes.inl>
#include <engine/graphics/renderer/backend/rendererbackend.hpp>

BEGIN_NAMESPACE

namespace Graphics
{
	class VulkanRenderer : public RendererBackend
	{
	public:
		DEFINE_RENDERER(RendererType::Vulkan);

		void Init() override;
		void Shutdown() override;

		void Resize(muint32 width, muint32 heigth) override;
		void BeginFrame(Time time) override;
		void EndFrame(Time time) override;
	private:
		VkInstance m_Instance;
		VkAllocationCallbacks* m_Allocator = nullptr;
	};
}

END_NAMESPACE