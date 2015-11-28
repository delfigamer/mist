local modname = ...
local ebase = require('exl.node.expr.base')
local estring = ebase:module(modname)
local common
local fulltype
local stringti

function estring:init(pr)
	ebase.init(self, pr)
	self.value = pr.value
	self.constvalue = self
end

function estring:rcompile(stream)
	local name = stream:genname()
	stream:writetoken('v_string', name, self.value)
	return name
end

function estring:defstring(lp)
	return string.format('%q', self.value)
end

common = require('exl.common')
fulltype = require('exl.fulltype')
stringti = require('exl.system.ti.string')

estring.bsimplevalue = true
estring.fulltype = fulltype:create(stringti, false, true)
