local modname = 'host'
local host = package.modtable(modname)
local cbase = require('host.cbase')
local ffi = require('ffi')
local fileio = require('host.fileio')
local fileopenmode = require('host.fileopenmode')
local rsbin = require('host.rsbin')

function host.write(str)
	cbase:write(str)
end

local getchar_buffer = ffi.new('char[5]')
function host.getchar()
	cbase:getchar(getchar_buffer)
	return ffi.string(getchar_buffer)
end

local function tabconcat(...)
	local args = table.pack(...)
	for i = 1, args.count do
		args[i] = tostring(args[i])
	end
	return table.concat(args, '\t')
end

function _G.print(...)
	cbase:write(tabconcat(...) .. '\n')
end

function package.searchpath(modname, template)
	modname = string.gsub(modname, '%.', '/')
	local errstr = {}
	for dir in string.gmatch(template, '[^;]+') do
		local path = string.gsub(dir, '%?', modname)
		if rsbin:isfileavailable(path) then
			return path
		else
			table.append(errstr, '\n\tno file \'')
			table.append(errstr, path)
			table.append(errstr, '\'')
		end
	end
	return nil, table.concat(errstr)
end

local buffer = ffi.new('char[0x1000]')

local function file_loader(io)
	local offset = 0ULL
	return function()
		local task = io:startread(offset, 0x1000, buffer, true)
		while not task:isfinished() do
			cbase:yield()
		end
		local result = task:getresult()
		if result == 0 then
			local err = task:geterror()
			if err then
				error(err)
			else
				return
			end
		else
			offset = offset + result
			return ffi.string(buffer, result)
		end
	end
end

package.loaders[1] = function(modname)
	return package.preload[modname]
end

package.loaders[2] = function(modname)
	local path, perr = package.searchpath(modname, package.path)
	if not path then
		return perr
	end
	local iosuc, ioret = pcall(fileio.create, fileio, path, fileopenmode.read)
	if not iosuc then
		return ioret
	end
	local chunk, err = load(file_loader(ioret), path, 'bt', env)
	ioret:release()
	if chunk then
		return chunk
	else
		return err
	end
end

do
	local testdw = ffi.typeof([[
		union {
			uint64_t d;
			struct {
				uint32_t l, h;
			};
		}
	]])
	if testdw(1).l == 0 then
		ffi.cdef([[
			typedef union {
				uint64_t d;
				int64_t ds;
				struct {
					uint32_t h, l;
				};
			} dword_t;
		]])
	else
		ffi.cdef([[
			typedef union {
				uint64_t d;
				int64_t ds;
				struct {
					uint32_t l, h;
				};
			} dword_t;
		]])
	end
end
