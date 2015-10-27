local modname = ...
module(modname, package.seeall)
local exception = require('base.exception')
local format = require('base.format')
local utility = require('base.utility')

exlerror = exception:derive()

function exlerror:derive(target, fstr)
	target = exception.derive(self, target)
	target.format = fstr
	return target
end

function exlerror:init(it, depth)
	exception.init(self, nil, depth and depth+1 or 2)
	self.it = it
	self.it.self = self
end

function exlerror.instmeta:__tostring()
	return format.format(self.format, self.it)
end

local function err(fstr)
	local class = exlerror:derive(nil, fstr..'<<self.traceback>>')
	return utility.method(class, class.create)
end

cast_failure = err('cannot cast <<value>> of type <<stype>> '..
	'to <<ttype>>')
const_expected = err('value <<value>> is not a constant')
scope_invalid = err('values of type <<type>> cannot be scoped')
scope_undefined = err('<<value>> of type <<type>> does not '..
	'have a member <<name>>')
symbol_redefined = err('symbol already defined: <<name>>')
symbol_undefined = err('undefined symbol: <<name>>')
