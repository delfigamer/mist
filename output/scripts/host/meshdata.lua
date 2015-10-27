local modname = ...
local ref = require('host.ref')
local meshdata = ref:module(modname, 'meshdata', 'MeshData')
local databuffer = require('host.databuffer')
local ffi = require('ffi')
local ffipure = require('base.ffipure')
local host = require('host')
local ml = require('host.methodlist').graphics

if ml == nil then
	error('graphics are not supported')
end

local meshbuffer = ffipure:derive(nil, modname .. ':meshbuffer')

meshbuffer.fields = 'MeshBuffer* ptr; MeshData* meshdata;'

function meshbuffer:release()
	if self.ptr ~= nil then
		ml.meshdata_unlock(self.meshdata, self.ptr)
		self.ptr = nil
	end
end

function meshbuffer:getvertexdata()
	local ptr = ml.meshbuffer_getvertexdata(self.ptr)
	if ptr == nil then
		host.checkerror()
		return nil
	else
		return databuffer:wrap(ptr)
	end
end

function meshbuffer:setvertexdata(db)
	if not ml.meshbuffer_setvertexdata(self.ptr, db and db.ptr) then
		host.checkerror()
	end
end

function meshbuffer:getindexdata()
	local ptr = ml.meshbuffer_getindexdata(self.ptr)
	if ptr == nil then
		host.checkerror()
		return nil
	else
		return databuffer:wrap(ptr)
	end
end

function meshbuffer:setindexdata(db)
	if not ml.meshbuffer_setindexdata(self.ptr, db and db.ptr) then
		host.checkerror()
	end
end

function meshdata:create()
	local ptr = ml.meshdata_new()
	if ptr ~= nil then
		return self:new(ptr)
	else
		host.checkerror()
		return nil
	end
end

local pmeshbuffer = ffi.typeof('MeshBuffer**')

function meshdata:trylock()
	local mb = meshbuffer:new(nil, self.ptr)
	local mbptr = pmeshbuffer(mb)
	local value = ml.meshdata_trylock(self.ptr, mbptr)
	if value == 0 then
		host.checkerror()
		return nil
	else
		if value == 1 then
			return mb
		else
			return nil
		end
	end
end

function meshdata:lock()
	local mb = meshbuffer:new(nil, self.ptr)
	local mbptr = ffi.cast(pmeshbuffer, mb)
	while true do
		local value = ml.meshdata_trylock(self.ptr, mbptr)
		if value == 0 then
			host.checkerror()
			return nil
		else
			if value == 1 then
				return mb
			end
		end
	end
end
