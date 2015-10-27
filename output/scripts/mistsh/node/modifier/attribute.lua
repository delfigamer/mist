local modname = ...
local basemodifier = require('mistsh.node.modifier.base')
local attribute = basemodifier:module(modname)
local common = require('mistsh.common')

attribute.name = 'attribute'

function attribute:init(pr, context)
	error('explicit attributes are not supported')
	if context.stage ~= 'vertex' then
		error('attributes must be declared at vertex')
	end
	basemodifier.init(self, pr, context)
	if context.attributes[pr._source] == nil then
		error('invalid attribute source')
	elseif context.attributes[pr._source] then
		error('attribute source already bound')
	end
	self.source = pr._source
end

function attribute:defstring(lp)
	return string.format('attribute %s', self.source)
end

function attribute:apply(var)
	if var.value then
		error('attributes cannot be initialized')
	end
	self.context.attributes[self.source] = var
	self.context.attributevars[var.name] = var
end

if common.target == 'hlsl_3_0' then
	local semantics = {
		position = 'POSITION',
		texcoord1 = 'TEXCOORD0',
		texcoord2 = 'TEXCOORD1',
		color = 'COLOR0',
	}
	function attribute:buildvar(var, section)
		if section ~= 'vheader' and section ~= 'fheader' then
			return ''
		end
		return var.type:build() .. ' ' .. var.name .. ': ' .. semantics[self.source] .. ';'
	end
elseif common.target == 'glsl_100' then
	function attribute:buildvar(var, section)
		if section ~= 'vheader' and section ~= 'fheader' then
			return ''
		end
		return 'attribute ' .. var.type:build() .. ' ' .. var.name .. ';'
	end
end
