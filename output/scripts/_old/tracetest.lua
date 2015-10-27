local path = '../'
local baselibfunc, err =
	loadfile(path .. 'scripts/system/baselib.lua')
if baselibfunc then
	baselibfunc(path)
else
	error(err)
end

local trace = require('base.trace')
trace.enable()

local function main()
	local function fact(n, factor)
		if n <= 1 then
			return factor
		else
			return fact(n-1, n * factor)
		end
	end
	
	local str = string.format('%i', fact(5, 1))
	print(str)
	
	local function ctfunc()
		print(1)
		coroutine.yield()
		print(2)
	end
	
	local t = coroutine.create(ctfunc)
	coroutine.resume(t)
	coroutine.resume(t)
end

local suc, err = pcall(main)
if not suc then
	print(err)
end

trace.disable()
print(600)
