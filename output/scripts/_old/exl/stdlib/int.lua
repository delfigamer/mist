local modname = ...
--local stdlibv = require('exl.stdlib')
local typebase = require('exl.type.base')
local intt = package.modtable(modname, typebase:create{})
--	context = stdlibv.context})

function intt:defstring(lp)
	return 'stdlib.int'
end

function intt:compile_luai(lp)
	return 'nil --[[stdlib.int]]'
end
