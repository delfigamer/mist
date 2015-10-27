local modname = ...
local util = package.modtable(modname)
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
