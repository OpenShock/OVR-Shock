#pragma once

#include "vr/transform.h"

#include <glm/glm.hpp>

namespace ZapMe
{
	struct TrackedDevice
	{
	public:
		TrackedDevice();
		~TrackedDevice();

		Transform transform;
	};
}