local modname = ...
module(modname, package.seeall)
local common

function createnode(self, pr, ...)
	if pr.infix == '*' then
		pr.name = 'expr.mul'
	elseif pr.infix == '**' then
		pr.name = 'expr.muld'
	else
		pr.name = 'expr.div'
	end
	return common.createnode(pr, ...)
end

common = require('mistsh.common')
