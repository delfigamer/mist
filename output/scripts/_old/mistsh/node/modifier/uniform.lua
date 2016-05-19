local modname = ...
local basemodifier = require('mistsh.node.modifier.base')
local uniform = basemodifier:module(modname)
local common = require('mistsh.common')

uniform.name = 'uniform'

function uniform:init(pr, context)
	error('explicit uniforms are not supported')
	if context.stage == 'common' then
		error('uniforms cannot be declared at common')
	end
	basemodifier.init(self, pr, context)
end

function uniform:apply(var)
	if var.value then
		error('attributes cannot be initialized')
	end
	table.append(self.context.uniforms, var)
end

if common.target == 'hlsl_3_0' then
	function uniform:buildvar(var, section)
		if section ~= 'vheader' and section ~= 'fheader' then
			return ''
		end
		return var.type:build() .. ' ' .. var.name .. ';'
	end
elseif common.target == 'glsl_100' then
	function uniform:buildvar(var, section)
		if section ~= 'vheader' and section ~= 'fheader' then
			return ''
		end
		return 'uniform ' .. var.type:build() .. ' ' .. var.name .. ';'
	end
end
