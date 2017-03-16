local modname = ...
local index = package.modtable(modname)
local databuffer = require('host.databuffer')
local exlast = require('host.exlast')
-- local exlcontext = require('host.exlcontext')
local exlnode = require('host.exlnode')
local exlparser = require('host.exlparser')
local ffi = require('ffi')
local fileio = require('rs.fileio')
local invoke = require('base.invoke')
local iostream = require('rs.iostream')
local list = require('list')
local rsutil = require('rs.util')

local function readdata(path)
	local fio = fileio:create(path, 'r')
	local stream = iostream:create(fio)
	local parts = {}
	local totallength = 0
	while true do
		local buf = databuffer:create(0, 16, nil)
		table.append(parts, buf)
		local advance = stream:read(16, buf:getdata())
		buf:setlength(advance)
		totallength = totallength + advance
		if advance < 16 then
			break
		end
	end
	stream:release()
	fio:release()
	return parts
end

local function dbtostr(db)
	return ffi.string(db:getdata(), db:getlength())
end

-- while true do
	do
		local env = require('list.env'):create({
			}, exlast:getnamemap(), exlast:getcontext())
		local filename = 'scripts/test.exl'
		local parts = invoke(readdata, filename)
		local source = ffi.new('char const*[?]', #parts)
		local slens = ffi.new('size_t[?]', #parts)
		for i, buf in ipairs(parts) do
			source[i-1] = ffi.cast('char const*', buf:getdata())
			slens[i-1] = buf:getlength()
		end
		local se = exlparser:parse(source, slens, #parts, filename, 3)
		local str = list.outputform(se, env)
		rsutil.savetextfile(str, 'test.lua')
		-- local context = exlcontext:create()
		local node = exlnode:create(se)
		print(dbtostr(node:getdefstring()))
		-- print(dbtostr(context:getdefstring()))
	end
	collectgarbage()
-- end
print()
