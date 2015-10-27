local modname = ...
local object = require('base.object')
local packageitem = object:module(modname)
local formattable = require('rs.formattable')
local resourcetable = require('rs.resourcetable')
local rsfunc = require('rs.func')

function packageitem:init(package, hostpi, mode, type, format)
	self.hostpi = hostpi
	type = type or assert(resourcetable[hostpi:gettype()], 'unknown resource type')
	format = format or assert(formattable[hostpi:getformat()], 'unknown resource format')
	self.resource = type:create(self, format, mode)
	local uuid = hostpi:getuuid()
	package.itemtable[rsfunc.uuidtoname(uuid)] = self
	local name = hostpi:getname()
	if name then
		package.itemtable[name] = self
	end
end

function packageitem:getuuid()
	return self.hostpi:getuuid()
end

function packageitem:getitemio(create)
	return self.hostpi:getitemio(create)
end

function packageitem:gettag()
	return self.hostpi:gettag()
end

function packageitem:settag(tag)
	return self.hostpi:settag(tag)
end

function packageitem:getmetaio(create)
	return self.hostpi:getmetaio(create)
end

function packageitem:getmetalength()
	return self.hostpi:getmetalength()
end

function packageitem:setmetalength(length)
	self.hostpi:setmetalength(length)
end

function packageitem:gettype()
	return self.hostpi:gettype()
end
