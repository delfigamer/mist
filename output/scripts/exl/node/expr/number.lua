local modname = ...
local ebase = require('exl.node.expr.base')
local enumber = ebase:module(modname)
local common
local fulltype
local numberti

function enumber:init(pr)
	ebase.init(self, pr)
	self.value = pr.value
	self.constvalue = self
end

function enumber:rcompile(stream)
	local name = stream:genname()
	stream:writetoken('v_number', name, self.value)
	return name
end

function enumber:defstring(lp)
	return tostring(self.value)
end

common = require('exl.common')
fulltype = require('exl.fulltype')
numberti = require('exl.system.ti.number')

enumber.bsimplevalue = true
enumber.fulltype = fulltype:create(numberti, false, true)
