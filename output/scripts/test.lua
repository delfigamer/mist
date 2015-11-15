local invoke = require('base.invoke')

local function main()
	local fileio = require('rs.fileio')
	local iostream = require('rs.iostream')
	local object = require('base.object')
	local tokenstream = require('exl.parser.tokenstream')
	local syntax = require('exl.parser.syntax')
	local env = {}
	function env:log(...)
		print('=', ...)
	end
	local io = fileio:create('scripts/test.exl', 'r')
	local stream = iostream:create(io)
	local ts = tokenstream:create(stream, env)
	local body = syntax.block(ts, table.makeset{'end'})
	body:build(env)
	print(body)
	print(body.namespace)
end

invoke(function() assert(pcall(main)) end)
