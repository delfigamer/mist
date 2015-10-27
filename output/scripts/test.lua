if not _PATH then
	local path = '../'
	local baselibfunc, err =
		loadfile(path .. 'scripts/system/baselib.lua')
	if baselibfunc then
		baselibfunc(path)
	else
		error(err)
	end
end

local function main()
	local peg = require('peg')
	local rule = peg.loadfile(_PATH .. 'scripts/exl/syntax.peg')
	local source =
		assert(io.open(_PATH .. 'scripts/test.exl', 'r'))
		:read('*a')
		:gsub('\t', '   ')
	local stream = peg.stream:create(source)
	local row, col, tree = rule:invoke(stream)
	-- print(tree)
	local exlcommon = require('exl.common')
	local body = exlcommon.createpoint(tree)
	print(body)
end

assert(pcall(main))
