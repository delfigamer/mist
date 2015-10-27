local modname = ...
local base = require('rs.resource.base')
local raw = base:module(modname)
local func = require('rs.func')
local package = require('rs.package')
local rawformat = require('rs.format.raw')

raw.format = rawformat

function raw:init(item, format, mode)
	self.typeid = item:gettype()
	self.item = item
	local uuid = item:getuuid()
	self.readonly = package.moderomap[mode]
	if package.modeloadmap[mode] then
	else
		self.metadata = {}
		self.ischanged = true
		self:createnew()
	end
end

function raw:createnew()
	self.data = ''
end

function raw:getdata()
	self:finishloading()
	return self.data
end

function raw:setdata(data)
	self:touchwrite()
	self:finishloading()
	self.data = data
end
