local modname = ...
local scopeex
local point
local pointset = require('exl.point.pointset')
local indexpoint = package.modtable(modname)

pointset['expr.index'] = indexpoint

function indexpoint:create(pr, context)
	local base = point:create(pr._base, context)
	for i, index in ipairs(pr._indices) do
		if index.name == 'expr.index.ident' then
			base = scopeex:create(index, context, {base = base})
		end
	end
	return base
end

scopeex = require('exl.point.expr.scope')
point = require('exl.point.base')
