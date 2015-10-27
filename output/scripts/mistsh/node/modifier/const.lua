local modname = ...
local basemodifier = require('mistsh.node.modifier.base')
local const = basemodifier:module(modname)
local common = require('mistsh.common')

const.name = 'const'

function const:apply(var)
	if not var.value then
		error('const must be initialized')
	end
	if self.context.consts then
		table.append(self.context.consts, var)
	end
end

if common.target == 'hlsl_3_0' then
	function const:buildvar(var)
		return 'static const ' .. var.type:build() .. ' ' .. var.name .. ' = ' .. var.value:build() .. ';'
	end
elseif common.target == 'glsl_100' then
	function const:buildvar(var)
		return 'const ' .. var.type:build() .. ' ' .. var.name .. ' = ' .. var.value:build() .. ';'
	end
end
