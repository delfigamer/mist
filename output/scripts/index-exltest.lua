local modname = ...
local index = package.modtable(modname)
local window = require('host.window')
local invoke = require('base.invoke')

local function main()
	local context = require('exl.context')
	local ilstream = require('exl.il.stream')
	local ilstat = require('exl.il.stat')
	local listptype = require('list.ptype')
	local scontext = require('exl.system.context')
	local fileio = require('rs.fileio')
	local iostream = require('rs.iostream')
	local object = require('base.object')
	local rsutil = require('rs.util')
	local rsscalars = require('rs.scalars')
	local tokenstream = require('exl.parser.tokenstream')
	-- local translate = require('exl.bytecode.translate')
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
	ourcontext.nameprefix = 'test.'
	body:build(ourcontext)
	local block = ilstream:create()
	body:compile(block)
	block:compact()
	rsutil.savetextfile(block:defstring(''), 'testr.exlc')
	rsutil.saveptype(listptype, block.body, 'testr.exlb')
	print(ilstat.bodydefstring(block.body, '', '\t'))
	-- print(translate(block))
end

local function action()
	invoke(function() assert(pcall(main)) end)
end

assert(pcall(action))
