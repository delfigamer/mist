local modname = ...
local node = require('mistsh.node')
local type = node:module(modname)
local common = require('mistsh.common')

function type:init(pr, context)
	node.init(self, pr, context)
	self.name = pr._name
end

function type:defstring(lp)
	return self.name
end

local targetname
if common.target == 'hlsl_3_0' then
	targetname = {}
elseif common.target == 'glsl_100' then
	targetname = {
		float2 = 'vec2',
		float3 = 'vec3',
		float4 = 'vec4',
		float4x4 = 'mat4',
	}
end

function type:build()
	return targetname[self.name] or self.name
end
