local modname = ...
local hostobject = require('host.hostobject')
local packageitem = hostobject:module(modname, 'packageitem', 'PackageItem')
local host = require('host')
local ffi = require('ffi')
local ml = require('host.methodlist').rsbin
local packageio = require('host.packageio')
local types = require('host.types')

function packageitem:getuuid()
	local uuid = types.itemuuid()
	if not ml.packageitem_getuuid(self.ptr, uuid) then
		host.checkerror()
		return nil
	end
	return uuid
end

function packageitem:getformat()
	local format = ml.packageitem_getformat(self.ptr)
	if format == 0 then
		host.checkerror()
	end
	return format
end

function packageitem:getitemio(create)
	local ptr = ml.packageitem_getitemio(self.ptr, create)
	if ptr == nil then
		host.checkerror()
		return nil
	end
	return packageio:wrap(ptr)
end

function packageitem:getmetaio(create)
	local ptr = ml.packageitem_getmetaio(self.ptr, create)
	if ptr == nil then
		host.checkerror()
		return nil
	end
	return packageio:wrap(ptr)
end

function packageitem:gettag()
	local tag = types.itemtag()
	if not ml.packageitem_gettag(self.ptr, tag) then
		host.checkerror()
		return nil
	end
	return tag
end

function packageitem:settag(tag)
	if not ml.packageitem_settag(self.ptr, tag) then
		host.checkerror()
	end
end

function packageitem:getmetalength()
	local metalength = ml.packageitem_getmetalength(self.ptr)
	if metalength == 0 then
		host.checkerror()
	end
	return metalength
end

function packageitem:setmetalength(metalength)
	if not ml.packageitem_setmetalength(self.ptr, metalength) then
		host.checkerror()
	end
end

function packageitem:getsavetime()
	local savetime = ffi.new('int[6]')
	if not ml.packageitem_getsavetime(self.ptr, savetime) then
		host.checkerror()
		return nil
	end
	return savetime
end

function packageitem:setsavetime(st)
	if not ml.packageitem_setsavetime(self.ptr, st) then
		host.checkerror()
	end
end

function packageitem:gettype()
	local type = ml.packageitem_gettype(self.ptr)
	if type == nil then
		host.checkerror()
		return nil
	end
	return ffi.string(type)
end

function packageitem:getname()
	local name = ml.packageitem_getname(self.ptr)
	if name == nil then
		host.checkerror()
		return nil
	end
	return ffi.string(name)
end

function packageitem:setname(name)
	if not ml.packageitem_setname(self.ptr, name) then
		host.checkerror()
	end
end
