local modname = ...
local ebase = require('exl.node.expr.base')
local estring = ebase:module(modname)
local common
local fulltype
local stringti

local estringfulltype

function estring:init(pr)
	ebase.init(self, pr)
	self.value = pr.value
end

function estring:getfulltype()
	return estringfulltype
end

function estring:getconstvalue()
	return self
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

estringfulltype = fulltype:create(stringti, false, true)
