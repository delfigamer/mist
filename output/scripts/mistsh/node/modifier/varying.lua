local modname = ...
local basemodifier = require('mistsh.node.modifier.base')
local varying = basemodifier:module(modname)
local common = require('mistsh.common')

varying.name = 'varying'

function varying:init(pr, context)
	if context.stage ~= 'common' then
		error('varyings must be declared at common')
	end
	basemodifier.init(self, pr, context)
end

function varying:apply(var)
	if var.value then
		error('varyings cannot be initialized')
	end
	self.index = table.append(self.context.varyings, var)
	self.context.varyings[var.name] = var
end

if common.target == 'hlsl_3_0' then
	function varying:buildvar(var, section)
		if section ~= 'vheader' and section ~= 'fheader' then
			return ''
		end
		return var.type:build() .. ' ' .. var.name .. ': TEXCOORD' .. tostring(self.index-1) .. ';'
	end
elseif common.target == 'glsl_100' then
	function varying:buildvar(var, section)
		if section ~= 'vheader' and section ~= 'fheader' then
			return ''
		end
		return 'varying ' .. var.type:build() .. ' ' .. var.name .. ';'
	end
end
