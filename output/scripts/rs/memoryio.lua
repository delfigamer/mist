local modname = ...
local iowrapper = require(modname, 1, 'iowrapper')
local memoryio = iowrapper:module(modname)
local ffi = require('ffi')
local hostmemoryio = require('host.memoryio')

function memoryio:init(hostio)
	iowrapper.init(self, hostio or hostmemoryio:create())
end

function memoryio:readfromstream(stream)
	local offset = 0ULL
	local pblock = ffi.new('uint8_t*[1]')
	local pblength = ffi.new('size_t[1]')
	while true do
		self.hostio:getblock(offset, true, pblock, pblength)
		local result = stream:read(tonumber(pblength[0]), pblock[0])
		offset = offset + result
		if result < pblength[0] then
			break
		end
	end
	self.hostio:setsize(offset)
end

function memoryio:writetostream(stream)
	local offset = 0ULL
	local length = self.hostio:getsize()
	local pblock = ffi.new('uint8_t*[1]')
	local pblength = ffi.new('size_t[1]')
	while length > 0 do
		self.hostio:getblock(offset, false, pblock, pblength)
		if pblength[0] > length then
			pblength[0] = length
		end
		local result
		if pblock[0] ~= nil then
			result = stream:write(tonumber(pblength[0]), pblock[0])
		else
			result = stream:skip(tonumber(pblength[0]))
		end
		if result == 0 then
			error('i/o failure')
		end
		offset = offset + result
		length = length - result
	end
end
