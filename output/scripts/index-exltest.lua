local modname = ...
module(modname, package.seeall)
local window = require('host.window')
local mainconf = require('main')
local invoke = require('base.invoke')

local function main()
	local context = require('exl.context')
	local bcblock = require('exl.bytecode.block')
	local scontext = require('exl.system.context')
	local fileio = require('rs.fileio')
	local iostream = require('rs.iostream')
	local object = require('base.object')
	local rsutil = require('rs.util')
	local tokenstream = require('exl.parser.tokenstream')
	local translate = require('exl.bytecode.translate')
	local syntax = require('exl.parser.syntax')
	local filename = 'scripts/test.exl'
	local fio = fileio:create(filename, 'r')
	local stream = iostream:create(fio)
	local ts = tokenstream:create(stream, 'scripts/test.exl')
	local body = syntax.block(
		ts, 'block', syntax.stat.main, table.makeset{'end'})
	stream:release()
	fio:release()
	print(body, '\n')
	local ourcontext = context:create(scontext)
	ourcontext.namespace = 'test'
	body:build(ourcontext)
	local block = bcblock:create()
	body:compile(block)
	block:compact()
	rsutil.saveptype(block.ptype, block, 'testr.exlb')
	print(translate(block))
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
