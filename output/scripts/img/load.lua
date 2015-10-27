local modname = ...
local fileio = require('rs.fileio')
local fmat = require('img.fmat')
local invoke = require('base.invoke')
local iostream = require('rs.iostream')

local function load_file(path)
	local io = fileio:create(path, 'r')
	local stream = iostream:create(io)
	local header = fmat.headertype()
	if stream:read(16, header) ~= 16 then
		error('i/o error')
	end
	local ofmat = fmat:create(header)
	if stream:read(ofmat.datasize, ofmat.data) ~= ofmat.datasize then
		error('i/o error')
	end
	stream:release()
	io:release()
	return ofmat
end

local function load(it)
	invoke(load_file, it[1])
end

package.modtable(modname, load)
