local modname = ...
local node = require('mistsh.node')
local identrefexpr = node:module(modname)
local common = require('mistsh.common')

function identrefexpr:init(pr, context)
	node.init(self, pr, context)
	local target, index = string.match(pr._target, '(.*)%.(.*)')
	if target then
		self.target = target
		self.index = index
	else
		self.target = pr._target
	end
	if self.context.attributes and self.context.attributes[self.target] ~= nil then
		self.context.attributes[self.target] = true
	end
end

function identrefexpr:defstring(lp)
	return self.target .. (self.index and '.' .. self.index or '')
end

if common.target == 'hlsl_3_0' then
	function identrefexpr:build()
		local prefix
		if self.context.varyings[self.target] then
			prefix = 'mistsh_varyings.'
		elseif self.context.attributes and self.context.attributes[self.target] then
			prefix = 'mistsh_attributes.'
		else
			prefix = ''
		end
		return prefix .. self.target .. (self.index and '.' .. self.index or '')
	end
elseif common.target == 'glsl_100' then
	function identrefexpr:build()
		return self.target .. (self.index and '.' .. self.index or '')
	end
end
