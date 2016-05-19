local modname = ...
module(modname, package.seeall)
local ffi = require('ffi')
local ffipure = require('base.ffipure')
local host = require('host')
local utilsml = require('host.methodlist').utils

ffi.cdef(io.open(_PATH .. 'scripts/index-botfield/botfield.h'):read('*a'))
library = ffi.load(_PATH .. 'libbotfield.so')

library.botfield_cbase_seterrorfuncs(
	utilsml.cbase_geterror, utilsml.cbase_seterror)
library.botfield_cbase_setlogfunc(
	utilsml.cbase_log)

field = ffipure:derive({}, modname .. '.field')
field.fields = 'Botfield_Field* ptr;'

function field:create(width, height, matrix, script)
	local ptr = library.botfield_field_new(width, height, matrix, script)
	if ptr ~= nil then
		return self:new(ptr)
	else
		host.checkerror()
		return nil
	end
end

function field:release()
	library.botfield_field_delete(self.ptr)
end

function field:getbotpos()
	local ptr = library.botfield_field_getbotpos(self.ptr)
	if ptr == nil then
		host.checkerror()
	end
	return ptr
end

function field:setbotpos(x, y, yaw)
	if not library.botfield_field_setbotpos(self.ptr, x, y, yaw) then
		host.checkerror()
	end
end

function field:setenginepos(dx, dy)
	if not library.botfield_field_setenginepos(self.ptr, dx, dy) then
		host.checkerror()
	end
end

function field:setsensorpos(lx, ly, rx, ry)
	if not library.botfield_field_setsensorpos(self.ptr, lx, ly, rx, ry) then
		host.checkerror()
	end
end

function field:tick(dt)
	if not library.botfield_field_tick(self.ptr, dt) then
		host.checkerror()
	end
end
