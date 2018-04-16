local modname = 'host'
local host = package.modtable(modname)
local ffi = require('ffi')
local object = require('base.object')
local filestorage = require('rsbin.filestorage')
local fileopenmode = require('rsbin.fileopenmode')
local isfileavailable = require('rsbin.isfileavailable')
local write = require('utils.write')
local getchar = require('utils.getchar')
local yield = require('utils.yield')

function host.write(...)
	return write(...)
end

local getchar_buffer = ffi.new('char[5]')
function host.getchar()
	getchar(getchar_buffer)
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
	write(tabconcat(...) .. '\n')
end

function package.searchpath(modname, template)
	modname = string.gsub(modname, '%.', '/')
	local errstr = {}
	for dir in string.gmatch(template, '[^;]+') do
		local path = string.gsub(dir, '%?', modname)
		if isfileavailable(path) then
			return path
		else
			table.append(errstr, '\n\tno file \'')
			table.append(errstr, path)
			table.append(errstr, '\'')
		end
	end
	return nil, table.concat(errstr)
end

local function storage_loader(storage)
	local offset = 0ull
	return function()
		local task = storage:startmap(offset, -1, true, false)
		if not task then
			return
		end
		while not task:poll() do
			yield()
		end
		local map = task:getmap()
		if map.ptr == nil or map.length == 0 then
			return
		end
		offset = offset + map.length
		local part = ffi.string(map.ptr, map.length)
		task:release()
		return part
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
	local iosuc, ioret = pcall(filestorage.create,
		filestorage, path, fileopenmode.read)
	if not iosuc then
		return ioret
	end
	local chunk, err = load(storage_loader(ioret), path, 'bt', _G)
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
