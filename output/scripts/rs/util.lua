local modname = ...
local util = package.modtable(modname)
local ffi = require('ffi')
local fileio = require('rs.fileio')
local invoke = require('base.invoke')
local iostream = require('rs.iostream')

util.assetpath = 'assets/?.masset;?.masset'

function util.nametopath(name)
	local template = util.assetpath:match('^(.-);') or util.assetpath
	local namepath = name:gsub('%.', '/')
	local path = template:gsub('%?', namepath)
	return path
end

function util.loadassetfile(class, name, path)
	local io = fileio:create(path, 'r')
	local stream = iostream:create(io)
	local asset = class:load(name, stream)
	return asset
end

function util.loadasset(class, name)
	local path, perr = package.searchpath(name, util.assetpath)
	if path then
		return util.loadassetfile(class, name, path)
	else
		error(perr)
	end
end

function util.saveassetfile(asset, path)
	local io = fileio:create(path, 'w')
	local stream = iostream:create(io)
	asset:save(stream)
	stream:release()
	io:release()
end

function util.saveasset(asset)
	local path = util.nametopath(asset.name)
	return util.saveassetfile(asset, path)
end

function util.loadptype(ptype, path)
	local io = fileio:create(path, 'r')
	local stream = iostream:create(io)
	local object = invoke(ptype.read, ptype, stream)
	stream:release()
	io:release()
	return object
end

function util.saveptype(ptype, object, path)
	local io = fileio:create(path, 'w')
	local stream = iostream:create(io)
	invoke(ptype.write, ptype, object, stream)
	stream:release()
	io:release()
end

local function loadtextfile_c(stream)
	local cbuf = ffi.new('char[0x10000]')
	local parts = {}
	while true do
		local length = stream:read(0x10000, cbuf)
		if length > 0 then
			table.append(parts, ffi.string(cbuf, length))
		end
		if length < 0x10000 then
			break
		end
	end
	return table.concat(parts)
end

function util.loadtextfile(path)
	local io = fileio:create(path, 'r')
	local stream = iostream:create(io)
	local text = invoke(loadtextfile_c, stream)
	stream:release()
	io:release()
	return text
end

function util.savetextfile(text, path)
	local io = fileio:create(path, 'w')
	local stream = iostream:create(io)
	if invoke(stream.write, stream, #text, text) ~= #text then
		error('i/o failure')
	end
	stream:release()
	io:release()
end
