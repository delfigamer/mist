local modname = ...
local crc32 = require('crc32')
local list = require('list')
local scalars = require('rs.scalars')

local function listptype_write(self, it, stream)
	local headindex = list.headindex(it, self.env)
	if headindex < 0x40000000 then
		scalars.uint32:write(headindex, stream)
		scalars.uint32:write(list.getlength(it), stream)
		for i, elem in list.elements(it) do
			listptype_write(self, elem, stream)
		end
	elseif headindex == 0x40000000 then
		scalars.uint32:write(0x40000000, stream)
	elseif headindex == 0x50000000 then
		scalars.uint32:write(0x50000000, stream)
		scalars.double:write(list.asatom(it), stream)
	elseif headindex == 0x60000000 then
		scalars.uint32:write(0x60000000, stream)
		scalars.uint32:write(list.asatom(it) and 1 or 0, stream)
	elseif headindex == 0x70000000 then
		scalars.uint32:write(0x70000000, stream)
		scalars.string:write(list.asatom(it), stream)
	else
		error('invalid element type')
	end
end

local function listptype_read(self, stream)
	local headindex = scalars.uint32:read(stream)
	if headindex < 0x40000000 then
		local it = {[0] = self.env:namestring(headindex)}
		local length = scalars.uint32:read(stream)
		for i = 1, length do
			it[i] = listptype_write(self, stream)
		end
		return list.pack(it, self.env)
	elseif headindex == 0x40000000 then
		return nil
	elseif headindex == 0x50000000 then
		return scalars.double:read(stream)
	elseif headindex == 0x60000000 then
		return scalars.uint32:read(stream) ~= 0
	elseif headindex == 0x70000000 then
		return scalars.string:read(stream)
	else
		error('invalid element type')
	end
end

local function createlistptype(env, typename)
	return {
		typename = typename,
		typeid = crc32(typename),
		env = env,
		write = listptype_write,
		read = listptype_read,
	}
end

package.modtable(modname, createlistptype)
