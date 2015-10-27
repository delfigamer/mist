local modname = ...
local node = require('mistsh.node')
local returnstat = node:module(modname)
local common = require('mistsh.common')

function returnstat:init(pr, context)
	node.init(self, pr, context)
	self.value = common.createnode(pr._value, context)
end

function returnstat:defstring(lp)
	return string.format('return %s',
		self.value:defstring(lp .. self.lpindent))
end

if common.target == 'hlsl_3_0' then
	function returnstat:build(section)
		return 'return ' .. self.value:build(section) .. ';'
	end
elseif common.target == 'glsl_100' then
	function returnstat:build(section)
		if section == 'vbody' then
			return 'gl_Position = ' .. self.value:build(section) .. ';'
		elseif section == 'fbody' then
			return 'gl_FragColor = ' .. self.value:build(section) .. ';'
		end
	end
end
