local modname = ...
module(modname, package.seeall)
local common

function createnode(self, pr, ...)
	if pr.infix == '+' then
		pr.name = 'expr.add'
	else
		pr.name = 'expr.sub'
	end
	return common.createnode(pr, ...)
end

common = require('mistsh.common')
