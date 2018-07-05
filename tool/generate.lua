local modname = ...
local env = require('env')

-- entry point: generate(it)
-- it field:
--   target
--   source
--   info

local pageenvbase = {
	assert = assert,
	error = error,
	ipairs = ipairs,
	print = print,
	sources = sources,
	string = string,
	table = table,
}

function pageenvbase.path(str)
	return (string.gsub(str, '/+', '\\'))
end

function pageenvbase.echo(...)
	local arg = table.pack(...)
	for i = 1, arg.count do
		if arg[i] ~= nil then
			coroutine.yield(tostring(arg[i]))
		end
	end
end

function pageenvbase.sep(sep)
	local next
	return function()
		if next then
			pageenvbase.echo(sep)
		end
		next = true
	end
end

local function translate(env, modulesource)
	local func = assert(load(modulesource, 'generator', 't', env))
	local content = {}
	local thread = coroutine.create(func)
	local suc, ret = coroutine.resume(thread)
	while true do
		if not suc then
			error(ret)
		elseif ret then
			table.append(content, ret)
		end
		if coroutine.status(thread) == 'suspended' then
			suc, ret = coroutine.resume(thread)
		else
			break
		end
	end
	return table.concat(content)
end

local function getfilecontents(path)
	local stream = assert(io.open(env.path(path), 'r'))
	local text = stream:read('*a')
	stream:close()
	return text
end

local function setfilecontents(path, text)
	local stream = assert(io.open(env.path(path), 'w'))
	stream:write(text)
	stream:close()
end

local function generate(it)
	local srctext = getfilecontents(it.source)
	local fenv = setmetatable({}, {__index = pageenvbase})
	fenv.info = it.info
	local modulesource = ''
	while true do
		local cstart, cmd, body, cend =
			string.match(srctext, '()<%?(%S*)(.-)%?>()')
		if cstart then
			if cstart > 1 then
				modulesource = modulesource .. string.format(
					'echo(%q);', string.sub(srctext, 1, cstart-1))
			end
			if cmd == 'lua' then
				modulesource = modulesource .. body
			elseif cmd == 'le' then
				modulesource = modulesource .. 'echo(' .. body .. ');'
			else
				modulesource = modulesource .. string.format(
					'echo(%q);', '<?' .. cmd .. body .. '?>')
			end
			srctext = string.sub(srctext, cend)
		else
			modulesource = modulesource .. string.format(
				'echo(%q);', srctext)
			break
		end
	end
	local result = translate(fenv, modulesource)
	setfilecontents(it.target, result)
end

package.modtable(modname, generate)
