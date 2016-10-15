local modname = ...
local object = require(modname, 2, 'object')
local stream = object:module(modname)
local ldefstring = require('list.defstring')
local compact = require(modname, 1, 'compact')

function stream:init(body)
	self.lastname = 0
	self.body = body or {}
end

function stream:genname()
	self.lastname = self.lastname + 1
	return self.lastname
end

function stream:append(t)
	table.append(self.body, t)
end

function stream:compact()
	compact(self)
end

function stream:defstring(lp)
	return ldefstring(self.body, lp)
end
