local modname = ...
local node = require('mistsh.node')
local assignmentstat = node:module(modname)
local common = require('mistsh.common')

function assignmentstat:init(pr, context)
	node.init(self, pr, context)
	local target, index = string.match(pr._target, '(.*)%.(.*)')
	if target then
		self.target = target
		self.index = index
	else
		self.target = pr._target
	end
	self.source = common.createnode(pr._source, context)
end

function assignmentstat:defstring(lp)
	return self.target .. (self.index and '.' .. self.index or '')
		.. ' = ' .. self.source:defstring(lp .. self.lpindent)
end

if common.target == 'hlsl_3_0' then
	function assignmentstat:build(section)
		local tprefix
		if self.context.varyings[self.target] then
			tprefix = 'mistsh_varyings.'
		else
			tprefix = ''
		end
		return tprefix .. self.target  .. (self.index and '.' .. self.index or '')
			.. ' = ' .. self.source:build(section) .. ';'
	end
elseif common.target == 'glsl_100' then
	function assignmentstat:build(section)
		return self.target .. (self.index and '.' .. self.index or '')
			.. ' = ' .. self.source:build(section) .. ';'
	end
end
