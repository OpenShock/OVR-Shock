#pragma once

#include <glm/glm.hpp>
#include <vector>

namespace ZapMe
{
	struct Transform
	{
		glm::vec3 position;
		glm::vec3 rotation;
	};

	enum class TrackedDeviceType
	{
		HMD,
		ControllerLeft,
		COntrollerRight,
		Tracker
	};
	struct TrackedDevice
	{
		TrackedDeviceType type;
		Transform transform;
	};

	struct Overlay
	{
		Transform transform;
		glm::vec2 uvSize;
	};

	class SceneManager
	{
	public:
		SceneManager();
		~SceneManager();

	private:
		std::vector<TrackedDevice> m_deviceTransforms;
		std::vector<Transform> m_overlayTransforms;
	};
}