local modname = ...
local ebase = require('exl.node.expr.base')
local evoid = ebase:module(modname)
local common

function evoid:init(pr)
	ebase.init(self, pr)
end

function evoid:defstring(lp)
	return 'void'
end

common = require('exl.common')
