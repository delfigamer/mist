local modname = ...
module(modname, package.seeall)
local debug = require('debug')
local serialize = require('base.serialize').serialize

local function tidtopath(tid)
	if type(tid) == 'string' then
		return tid
	else
		return tostring(tid):match('0x%x*')
	end
end

local function calldepth(offset)
	local index = 1
	while true do
		local info = debug.getinfo(index, '')
		if not info then
			break
		end
		index = index + 1
	end
	return index - 1 - offset
end

local function funcentry(thread, func)
	if func.what == 'C' then
		thread.logfile:write(string.format('\n%s= %s :: %s\n%s>',
			string.rep('\t', thread.depth), func.short_src, func.name, string.rep('\t', thread.depth)))
	else
		thread.depth = thread.depth + 1
		thread.stack[thread.depth] = func
		thread.logfile:write(string.format('\n%s+ %s :: %s\n%s>\t%i',
			string.rep('\t', thread.depth - 1), func.short_src, func.name, string.rep('\t', thread.depth), func.linedefined))
	end
	thread.logfile:flush()
end

local function funcexit(thread)
	local func = thread.stack[thread.depth]
	thread.logfile:write(string.format('\n%s- %s :: %s\n%s>',
		string.rep('\t', thread.depth - 1), func.short_src, func.name, string.rep('\t', thread.depth - 1)))
	thread.stack[thread.depth] = nil
	thread.depth = thread.depth - 1
	thread.logfile:flush()
end

local function funcline(thread, line)
	thread.logfile:write(string.format('\t%i', line))
end

local function threadstart(state, tid)
	state.threads[tid] = {
		depth = 1,
		stack = {},
		logfile = io.open(state.nameprefix .. tidtopath(tid) .. '.log', 'w')
	}
	return state.threads[tid]
end

local function checkdepth(thread)
	local depth = calldepth(2)
	if depth < thread.depth then
		for i = depth, thread.depth - 1 do
			funcexit(thread)
		end
	elseif depth > thread.depth then
		for i = thread.depth + 1, depth do
			funcentry(thread, debug.getinfo(depth - i + 3))
		end
	end
end

local function hook(state, event, line)
	local tid = coroutine.running() or 'main'
	local thread = state.threads[tid] or threadstart(state, tid)
	checkdepth(thread)
	if line then
		funcline(thread, line)
	end
end

function enable(it)
	local state = {
		nameprefix = it and it.nameprefix or (_PATH .. 'log/trace_'),
		threads = {}}
-- 	local thread = threadstart(state, 'main')
-- 	local stack = {}
-- 	local index = 0
-- 	while true do
-- 		local info = debug.getinfo(index)
-- 		if not info then
-- 			break
-- 		end
-- 		index = index + 1
-- 		stack[index] = info
-- 	end
-- 	for i = index, 1, -1 do
-- 		funcentry(thread, stack[i])
-- 	end
	debug.sethook(function(...) return hook(state, ...) end, 'crl')
end

function disable()
	debug.sethook()
	collectgarbage()
end
