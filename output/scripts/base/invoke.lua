local modname = ...
local utility = require('base.utility')

local function invoke_wrapper(func, ...)
	local ret = utility.pack(pcall(func, ...))
	if ret(1, 1) then
		return ret(2)
	else
		error(ret(2, 2))
	end
end

local function invoke(func, ...)
	local thread = coroutine.create(invoke_wrapper)
	local ret = utility.pack(coroutine.resume(thread, func, ...))
	while coroutine.status(thread) ~= 'dead' do
		ret = utility.pack(coroutine.resume(thread))
	end
	if ret(1, 1) then
		return ret(2)
	else
		error(ret(2, 2))
	end
end

return invoke
