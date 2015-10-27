local ppcasm = require('ppcasm')

local function main(sourcepath, targetpath, offset)
	local sourcefile = assert(io.open('../' .. sourcepath, 'r'))
	local source = sourcefile:read('*a')
	sourcefile:close()
	local result = ppcasm.assemble(source, tonumber(offset))
	local targetfile = assert(io.open('../' .. targetpath, 'wb'))
	targetfile:write(result)
	targetfile:close()
end

assert(pcall(main, ...))
