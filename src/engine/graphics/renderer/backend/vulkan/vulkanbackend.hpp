#pragma once
#include <defines.hpp>
#include <engine/graphics/renderer/backend/vulkan/vulkantypes.inl>
#include <engine/graphics/renderer/backend/rendererbackend.hpp>
#include <vector>

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

		void GetPlatformExtensions(std::vector<const char*>& platfromExtensions);

	private:
		VkInstance m_Instance;
		VkAllocationCallbacks* m_Allocator = nullptr;
#ifdef DEBUG
		VkDebugUtilsMessengerEXT m_DebugMessenger;
#endif
	};
}

END_NAMESPACE