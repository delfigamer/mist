local modname = ...
local object = require('base.object')
local package = object:module(modname)
local defer = require('base.defer')
local fileio = require('host.fileio')
local formattable = require('rs.formattable')
local host = require('host')
local hostpackage = require('host.package')
local packageitem = require('rs.packageitem')
local rawresource = require('rs.resource.raw')
local resourcetable = require('rs.resourcetable')
local rsfunc = require('rs.func')
local types = require('host.types')
require('rs.format')
require('rs.resource')

local _PATH = _PATH

package.modemap = {
	create = 'wb',
	edit = 'rb+',
	read = 'rb'}
package.modeloadmap = {
	edit = true,
	read = true}
package.moderomap = {
	read = true}

function package:init(path, mode, taskpriority)
	local io = fileio:create(path, assert(package.modemap[mode], 'invalid mode'))
	if package.modeloadmap[mode] then
		self.hostpackage, self.loading = hostpackage:load(io, taskpriority)
	else
		self.hostpackage = hostpackage:create(io)
	end
	self.readonly = package.moderomap[mode]
	self.itemtable = {}
end

function package:isready()
	return not self.loading or self.loading:finished()
end

function package:finishloading()
	if self.loading then
		self.loading:flush()
		self.loading = nil
	end
end

function package:touchwrite()
	if self.readonly then
		error('package is read-only')
	end
end

function package:createresource(uuid, name, type, format)
	self:touchwrite()
	self:finishloading()
	type = resourcetable[type]
	format = format and formattable[format] or type.format
	local hostpi = self.hostpackage:createitem(uuid, format.formatid, type.typeid, name)
	local pi = packageitem:create(self, hostpi, 'create')
	return pi.resource
end

function package:createrawresource(uuid, name, type, format)
	self:touchwrite()
	self:finishloading()
	local hostpi = self.hostpackage:createitem(uuid, format, type, name)
	local pi = packageitem:create(self, hostpi, 'create', rawresource, rawresource.format)
	return pi.resource
end

function package:loadresource(name, type)
	self:finishloading()
	if self.itemtable[name] then
		return self.itemtable[name].resource
	end
	local uuid = rsfunc.nametouuid(name)
	local hostpi = uuid and self.hostpackage:getitembyuuid(uuid) or self.hostpackage:getitembyname(name)
	if not hostpi then
		return nil
	end
	if type then
		local rtype = hostpi:gettype()
		if rtype ~= type then
			error(string.format('resource %s has type %s while %s is requested', name, rtype, type))
		end
	end
	local pi = packageitem:create(self, hostpi, self.readonly and 'read' or 'edit')
	return pi.resource
end

function package:save(taskpriority)
	self:touchwrite()
	self:finishloading()
	local tasks = {}
	local taskcount = 0
	for name, pi in pairs(self.itemtable) do
		if pi.resource.ischanged then
			tasks[name] = pi.resource:startsave(taskpriority)
			taskcount = taskcount + 1
		end
	end
	while taskcount ~= 0 do
		for name, task in pairs(tasks) do
			if task then
				if task:finished() then
					tasks[name] = false
					taskcount = taskcount - 1
				end
			end
		end
		defer.run()
	end
	self.hostpackage:save()
end

function package.items_iter(state)
	local uuid, item = state.nfunc(state.nstate)
	if not uuid then
		return
	end
	local uname = rsfunc.uuidtoname(uuid)
	local pi = state.package.itemtable[uname]
	local itemdata = {
		uuid = uuid,
		name = item:getname(),
		type = item:gettype(),
		format = item:getformat(),
		savetime = item:getsavetime(),
		resource = pi and pi.resource}
	return uname, itemdata
end

function package:items()
	self:finishloading()
	local state = {
		package = self}
	state.nfunc, state.nstate = self.hostpackage:items()
	return self.items_iter, state
end
