local modname = ...
local binaryexpr = require('mistsh.node.expr.binary')
local muldexpr = binaryexpr:module(modname)
local common = require('mistsh.common')

muldexpr.optoken = '**'

if common.target == 'hlsl_3_0' then
	function muldexpr:build(section)
		return
			'mul( ' .. self.lhs:build(section) ..
			', ' .. self.rhs:build(section) .. ' )'
	end
elseif common.target == 'glsl_100' then
	function muldexpr:build(section)
		return
			'( ' .. self.lhs:build(section) ..
			' * ' .. self.rhs:build(section) .. ' )'
	end
end
