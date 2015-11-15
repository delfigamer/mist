local modname = ...
module(modname, package.seeall)
local default

function createnode(pr)
	local success, result = pcall(require, 'exl.node.' .. pr.name)
	if success then
		return result:create(pr)
	else
		print(result)
		return default:create(pr)
	end
end

function defstring(node, lp)
	if not node then
		return '<error>'
	elseif type(node) == 'table' and node.defstring then
		return node:defstring(lp)
	else
		return tostring(node)
	end
end

default = require('exl.defaultnode')
