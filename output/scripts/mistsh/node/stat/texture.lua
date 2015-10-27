local modname = ...
local node = require('mistsh.node')
local texturestat = node:module(modname)
local common = require('mistsh.common')

function texturestat:init(pr, context)
	node.init(self, pr, context)
	self.name = pr._name
	self.index = tonumber(pr._index)
	if context.textures[self.index] then
		error('texture index is already bound')
	end
	context.textures[self.index] = self
	table.append(context.uniforms, self)
end

function texturestat:defstring(lp)
	return 'texture ' .. self.name
end

if common.target == 'hlsl_3_0' then
	function texturestat:build(section)
		if section == 'vheader' or section == 'fheader' then
			return 'sampler2D ' .. self.name .. ';'
		else
			return ''
		end
	end
elseif common.target == 'glsl_100' then
	function texturestat:build(section)
		if section == 'vheader' or section == 'fheader' then
			return 'uniform sampler2D ' .. self.name .. ';'
		else
			return ''
		end
	end
end
