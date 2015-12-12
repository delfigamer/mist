local modname = ...
local esymbolbase = require('exl.node.expr.symbolbase')
local ereference = esymbolbase:module(modname)
local common

function ereference:init(pr)
	esymbolbase.init(self, pr)
	self.target = pr.target
	esymbolbase.build(self, pc)
end

common = require('exl.common')
