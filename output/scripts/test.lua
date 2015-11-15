local invoke = require('base.invoke')

local function main()
	local fileio = require('rs.fileio')
	local iostream = require('rs.iostream')
	local object = require('base.object')
	local tokenstream = require('exl.parser.tokenstream')
	local syntax = require('exl.parser.syntax')
	local context = {}
	function context:log(...)
		print('=', ...)
	end
	local io = fileio:create('scripts/test.exl', 'r')
	local stream = iostream:create(io)
	local ts = tokenstream:create(stream)
	local node
	-- repeat
		-- local token = ts:gett(context)
		node = syntax.body(ts, context, {['end']=true})
		-- print(token)
	-- until token:gettype() == 'eof'
	-- print(object.defstring(tokens))
	print(node)
end

invoke(function() assert(pcall(main)) end)
