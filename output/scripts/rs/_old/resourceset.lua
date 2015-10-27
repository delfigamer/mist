local modname = ...
local object = require('base.object')
local resourceset = object:derive({}, modname)
local fileitem
local resourcetable
local rspackage

resourceset.packagesp = _PATH .. 'resources' .. package.dirsep .. '?.mpk'
resourceset.itemsp = _PATH .. 'resources' .. package.dirsep

function resourceset:init()
	self.packages = {}
	self.fileitems = {}
end

function resourceset:loadpackage(name)
	local path = assert(package.searchpath(name, resourceset.packagesp))
	self.packages[#self.packages + 1] = rspackage:create(path, 'read')
end

function resourceset:finishloading()
	for i, pack in ipairs(self.packages) do
		pack:finishloading()
	end
end

function resourceset:loadresource(name, type)
	if self.fileitems[name] then
		local item = self.fileitems[name]
		if type and item:gettype() ~= type then
			error(string.format('resource %s has type %s while %s is requested', name, item:gettype(), type))
		end
		return item.resource
	end
	if type then
		local typeclass = resourcetable[type]
		if typeclass.searchpath then
			local fpath, err = package.searchpath(name, resourceset.itemsp .. typeclass.searchpath)
			if fpath then
				local item = fileitem:create(fpath, nil, 'read', typeclass)
				self.fileitems[name] = item
				return item.resource
			end
		end
	end
	for i = #self.packages, 1, -1 do
		local res = self.packages[i]:loadresource(name, type)
		if res then
			return res
		end
	end
	return nil
end

package.modtable(modname, resourceset:create())

fileitem = require('rs.fileitem')
resourcetable = require('rs.resourcetable')
rspackage = require('rs.package')
