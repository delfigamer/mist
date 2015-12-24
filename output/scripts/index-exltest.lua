local modname = ...
module(modname, package.seeall)
local window = require('host.window')
local mainconf = require('main')
local invoke = require('base.invoke')

local function main()
	local exlbstream = require('exlb.stream')
	local scontext = require('exl.system.context')
	local fileio = require('rs.fileio')
	local iostream = require('rs.iostream')
	local object = require('base.object')
	local tokenstream = require('exl.parser.tokenstream')
	local syntax = require('exl.parser.syntax')
	local fio = fileio:create('scripts/test.exl', 'r')
	local stream = iostream:create(fio)
	local ts = tokenstream:create(stream, scontext.env)
	local body = syntax.block(ts, 'block', syntax.stat.main, table.makeset{'end'})
	print(body, '\n')
	body:build(scontext)
	local bs = exlbstream:create()
	bs:writetoken('d_filename', 'test.exl')
	body:compile(bs)
	print(bs)
	io.open('testr.lua', 'w'):write(tostring(bs))
end

local function action()
	invoke(function() assert(pcall(main)) end)
end

function register()
	local suc, err = pcall(action)
	if not suc then
		print(err)
	end
	window:finish()
end

function unregister()
end
