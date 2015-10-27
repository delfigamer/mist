local modname = ...
local object = require('base.object')
local syntax = require('exl.syntax')
local errors
local base = object:module(modname)

base.tabchar = syntax.tabchar
base.userprefix = 'user_'

function base:init(it)
	self.context = it.context
	self.visibility = self.context.visibility
	self.name = it.name
	if it.innername then
		self.innername = it.innername
	else
		if self.visibility == 'public' then
			self.innername = self.context.unit.innername..'.'..self.name
		else
			self.innername = self.userprefix..self.name
		end
	end
	if self.name then
		self.context.environment:set(self.name, self)
	end
end

function base:getconstvalue()
	return nil
end

function base:constvalue()
	error(errors.const_expected{value = self})
end

function base:defstring(lp)
	return '<entity "'..(self.name or '')..'">'
end

function base.instmeta:__tostring()
	return self:defstring('')
end

errors = require('exl.errors')
