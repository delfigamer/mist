local modname = ...
local object = require('base.object')
local resourcebase = object:module(modname)
-- local formattable = require('rs.format.table')
local func = require('rs.func')
local maintaskqueue = require('tasking.mainqueue')
local package = require('rs.package')
local resourceloadingtask = require('rs.resourceloadingtask')
local resourcesavingtask = require('rs.resourcesavingtask')
local resourcetable = require('rs.resourcetable')

function resourcebase:derive(target, modname, type)
	target = object.derive(self, target, modname)
	if type then
		resourcetable[type] = target
		target.typeid = type
	end
	return target
end

function resourcebase:init(item, format, mode, taskpriority)
	self.item = item
	self.format = format
	self.readonly = package.moderomap[mode]
	if package.modeloadmap[mode] then
		self.loading = self:startload(taskpriority)
	else
		self.metadata = {}
		self.ischanged = true
		self:createnew()
	end
end

function resourcebase:startload(taskpriority)
	local task = resourceloadingtask:create(taskpriority, self)
	maintaskqueue:insert(task)
	return task
end

function resourcebase:finishloading()
	if self.loading then
		self.loading:flush()
		self.loading = nil
	end
end

function resourcebase:touchwrite()
	if self.readonly then
		error('resource is read-only')
	end
end

function resourcebase:startsave(taskpriority)
	if self.readonly then
		error('resource is read-only')
	elseif self.loading then
		error('resource is loading')
	elseif self.saving then
		error('resource is saving')
	end
	self.ischanged = false
	return resourcesavingtask:create(taskpriority, self)
end

function resourcebase:afterloading()
end

function resourcebase:beforesaving()
end
