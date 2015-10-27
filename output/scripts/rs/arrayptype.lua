local modname = ...
local databuffer = require('host.databuffer')
local crc32 = require('crc32')
local scalars = require('rs.scalars')

local function arrayptype_write_scalar(self, arr, stream)
	local length = #arr
	scalars.int32:write(length, stream)
	for i = 1, length do
		self.elemtype:write(arr[i], stream)
	end
end

local function arrayptype_read_scalar(self, stream)
	local length = scalars.int32:read(stream)
	local arr = {}
	for i = 1, length do
		arr[i] = self.elemtype:read(stream)
	end
	return arr
end

local function arrayptype_write_object(self, arr, stream)
	local length = #arr
	scalars.int32:write(length, stream)
	for i = 1, length do
		local substream = stream:writesubstream()
		local suc, ret = pcall(self.elemtype.write, self.elemtype, arr[i], substream)
		substream:close()
		if not suc then
			error(ret)
		end
	end
end

local function arrayptype_read_object(self, stream)
	local length = scalars.int32:read(stream)
	local arr = {}
	for i = 1, length do
		local substream = stream:readsubstream()
		local suc, ret = pcall(self.elemtype.read, self.elemtype, substream)
		substream:close()
		if suc then
			arr[i] = ret
		else
			error(ret)
		end
	end
	return arr
end

local function createarrayptype(elemtype)
	local typename = modname .. '<' .. elemtype.typename .. '>'
	local isobject = not scalars[elemtype.typeid]
	return {
		typename = typename,
		typeid = crc32(typename),
		elemtype = elemtype,
		write = isobject and arrayptype_write_object or arrayptype_write_scalar,
		read = isobject and arrayptype_read_object or arrayptype_read_scalar,
	}
end

package.modtable(modname, createarrayptype)
