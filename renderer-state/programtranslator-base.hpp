#pragma once

#include <renderer-state/programtranslator.hpp>
#include <common/stringbuilder.hpp>
#include <common/ref.hpp>
#include <cstring>

namespace graphics
{
	namespace programtranslator
	{
		namespace valuetype
		{
			enum
			{
				float1,
				float2,
				float3,
				float4,
				float2x2,
				float3x3,
				float4x4,
				sampler2D,
			};
		}

		// back-end-dependent
		extern Ref< DataBuffer > const typestr[];

		struct Value: RefObject
		{
			int m_type;
			int m_usecount = 0;

			virtual StringBuilder getstring( StringBuilder* defs ) = 0;
		};
	}
}
