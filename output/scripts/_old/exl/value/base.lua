local modname = ...
local object = require('base.object')
local syntax = require('exl.syntax')
local errors
local base = object:module(modname)

base.tabchar = syntax.tabchar

function base:init(it)
	self.context = it.context
end

function base:getconstvalue()
	if self.isconst then
		return self
	end
end

function base:constvalue()
	return self:getconstvalue() or
		error(errors.const_expected{value = self})
end

function base:defstring(lp)
	return '<value>'
end

function base:compile_luai(lp)
	return 'nil --[[value]]'
end

function base.instmeta:__tostring()
	return self:defstring('')
end

errors = require('exl.errors')
