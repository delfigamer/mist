local modname = ...
local object = require('base.object')
local assetdata = object:module(modname)

function assetdata:init()
	self.fields = {}
end

function assetdata:load(stream)
	if self.reader then
		self.reader:halt()
	end
	self.reader = asyncreader:create(stream, assetparser, self)
end

function assetdata:save(stream)
	error('not supported')
end

function assetdata:onload()
end

function assetdata:get(...)
	return table.deepget(self.env, ...)
end

function assetdata:set(value, ...)
	table.deepset(self.env, value, ...)
end

function assetdata:optnumber(...)
	local val = self:get(...)
	return type(val) == 'number' and val
end

function assetdata:optstring(...)
	local val = self:get(...)
	return type(val) == 'string' and val
end

function assetdata:optdata(...)
	local val = self:get(...)
	return type(val) == 'cdata' and val['#host.databuffer'] and val
end
