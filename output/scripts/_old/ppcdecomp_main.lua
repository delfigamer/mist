local ppcdecomp = require('ppcdecomp')

local function main()
	local sourcefile = assert(io.open(_PATH .. ppcdecomp_config.sourcepath, 'rb'))
	local source = sourcefile:read('*a')
	sourcefile:close()
	local result = ppcdecomp.decompile(source, ppcdecomp_config)
	local targetfile = assert(io.open(_PATH .. ppcdecomp_config.targetpath, 'w'))
	targetfile:write(result)
	targetfile:close()
end

assert(pcall(main, ...))
