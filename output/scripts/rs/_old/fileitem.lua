local modname = ...
local object = require('base.object')
local fileitem = object:module(modname)
local extformatt = require('rs.format.exttable')
local extresourcet = require('rs.resource.exttable')
local formattable = require('rs.formattable')
local hostfile = require('host.fileio')
local resourcetable = require('rs.resourcetable')
local rsbin = require('host.rsbin')
local types = require('host.types')

local function getext(path)
	local pp, np
	np = path:find('.', 1, true)
	if not np then
		return ''
	end
	repeat
		pp = np
		np = path:find('.', pp + 1, true)
	until not np
	return path:sub(pp + 1)
end

local function u64totag(u64)
	local dw = types.dword(u64)
	return types.itemtag(dw.hi, dw.lo)
end

local function tagtou64(tag)
	local dw = types.dword{lo=tag.lo, hi=tag.hi}
	return dw.u64;
end

function fileitem:init(path, uuid, mode, type, format)
	self.path = path
	self.metapath = path .. '.metadata'
	self.uuid = uuid or types.itemuuid(0, 0)
	self.mode = mode
	if type and format then
		if _G.type(type) == 'string' then
			type = resourcetable[type]
		end
	else
		local ext = getext(path)
		local errstr = 'unknown extension: ' .. ext
		type = type or assert(extresourcet[ext], errstr)
		format = format or assert(extformatt[ext], errstr)
	end
	self.resource = type:create(self, format, mode)
end

function fileitem:getuuid()
	return self.uuid
end

function fileitem:getitemio(create)
	if create then
		self.io = hostfile:create(self.path, 'wb')
	else
		if rsbin.fileexists(self.path) then
			self.io = hostfile:create(self.path, 'rb+')
		end
	end
	return self.io
end

function fileitem:gettag()
	if rsbin.fileexists(self.path) then
		return u64totag(rsbin.filesize(self.path))
	else
		return types.itemtag(0, 0)
	end
end

function fileitem:settag(tag)
	if self.io then
		self.io:setsize(tagtou64(tag))
	end
end

function fileitem:getmetaio(create)
	if create then
		self.metaio = hostfile:create(self.metapath, 'wb')
	else
		if rsbin.fileexists(self.metapath) then
			self.metaio = hostfile:create(self.metapath, 'rb+')
		end
	end
	return self.metaio
end

function fileitem:getmetalength()
	if rsbin.fileexists(self.metapath) then
		local dw = types.dword(rsbin.filesize(self.metapath))
		assert(dw.hi == 0, 'file "' .. self.metapath .. '" is too large')
		return dw.lo
	else
		return 0
	end
end

function fileitem:setmetalength(length)
	if self.metaio then
		self.metaio:setsize(length)
	end
end

function fileitem:gettype()
	return self.type
end
