local modname = ...

local function invoke_wrapper(func, ...)
	local ret = table.pack(pcall(func, ...))
	if ret[1] then
		return table.unpack(ret, 2)
	else
		error(ret[2])
	end
end

local function invoke(func, ...)
	local thread = coroutine.create(invoke_wrapper)
	local ret = table.pack(coroutine.resume(thread, func, ...))
	while coroutine.status(thread) ~= 'dead' do
		ret = table.pack(coroutine.resume(thread))
	end
	if ret[1] then
		return table.unpack(ret, 2)
	else
		ret[2].traceback = ret[2].traceback .. debug.traceback('')
		error(ret[2])
	end
end

return invoke
