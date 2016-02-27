local modname = ...
local ref = require('host.ref')
local package = ref:module(modname, 'package', 'Package', 0x9f86e515)
local defaultqueue = require('host.defaultqueue')
local ffi = require('ffi')
local host = require('host')
local packageitem = require('host.packageitem')
local ml = require('host.methodlist').rsbin
local taskbase = require('host.taskbase')
local template = require('base.template')
local types = require('host.types')

function package:create(io)
	local ptr = ml.package_new(io and io.ptr or io)
	if ptr ~= nil then
		return self:new(ptr)
	else
		host.checkerror()
		return nil
	end
end

function package:load(io, taskpriority)
	local task = ffi.new('TaskBase*[1]')
	local ptr = ml.package_load(io and io.ptr or io, task, taskpriority or 0, defaultqueue.ptr)
	if ptr ~= nil then
		return self:new(ptr), taskbase:wrap(task[0])
	else
		host.checkerror()
		return nil
	end
end

function package:save()
	if not ml.package_save(self.ptr) then
		host.checkerror()
	end
end

function package:createitem(uuid, format, type, name)
	local ptr = ml.package_createitem(self.ptr, uuid, format, type, name)
	if ptr == nil then
		host.checkerror()
		return nil
	end
	return packageitem:wrap(ptr)
end

function package:getitembyuuid(uuid)
	local ptr = ml.package_getitembyuuid(self.ptr, uuid)
	if ptr == nil then
		host.checkerror()
		return nil
	end
	return packageitem:wrap(ptr)
end

function package:getitembyname(name)
	local ptr = ml.package_getitembyname(self.ptr, name)
	if ptr == nil then
		host.checkerror()
		return nil
	end
	return packageitem:wrap(ptr)
end

function package:isvalid()
	local r = ml.package_isvalid(self.ptr)
	if r == 0 then
		host.checkerror()
		return nil
	else
		return r == 1
	end
end

package.iterator = template.load('host.iterator', {
	mlmodule = 'host.methodlist',
	mlsubmodule = '.rsbin',
	mlprefix = 'package',
	ffitype = 'Package_iterator',
	targetmethod = 'items'})(package)

function package.iterator:deref()
	local uuid = types.itemuuid()
 	local item = ffi.new('PackageItem*[1]')
	if not ml.package_iterator_deref(self, uuid, item) then
		host.checkerror()
		return nil
	end
	return uuid, packageitem:wrap(item[0])
end
