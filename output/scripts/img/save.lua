local modname = ...
local fileio = require('rs.fileio')
local fmat = require('img.fmat')
local invoke = require('base.invoke')
local iostream = require('rs.iostream')

local function save_file(path, fmat)
	local io = fileio:create(path, 'w')
	local stream = iostream:create(io)
	if stream:write(16, fmat.header) ~= 16 then
		error('i/o error')
	end
	if stream:write(fmat.datasize, fmat.data) ~= fmat.datasize then
		error('i/o error')
	end
	stream:release()
	io:release()
end

local function save(it)
	invoke(save_file, it[1], it[2])
end

package.modtable(modname, save)
