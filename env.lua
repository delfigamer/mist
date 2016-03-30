local modname = ...
local env = package.modtable(modname)
local ffi = require('ffi')

function env.execute(str)
	return os.execute(str) == 0
end

if ffi.abi('win') then
	require(modname .. '-win')
end

function env.makepath(path)
	local subpath = {}
	for item in string.gmatch(path, '([^/]+)/') do
		table.append(subpath, item)
		local subpathstr = table.concat(subpath, '/')
		if not env.getfiletime(subpathstr) then
			env.mkdir(subpathstr)
		end
	end
	return true
end

function env.pathlist(list, prefix)
	prefix = prefix or ''
	local str = {}
	if not list then
		return ''
	elseif type(list) == 'string' then
		for item in string.gmatch(list, '[^;]+') do
			table.append(str, '"' .. prefix .. env.path(item) .. '" ')
		end
	else
		for i, item in ipairs(list) do
			str[i] = '"' .. prefix .. env.path(item) .. '" '
		end
	end
	return table.concat(str)
end

if not _G.toolchain or _G.toolchain == 'gcc' then
	function env.compile(t)
		return
			env.makepath(t.target) and
			env.execute('g++ \z
				-o "' .. env.path(t.target) .. '" \z
				"' .. env.path(t.source) .. '" \z
				-c \z
				-O2 \z
				-Wall -Werror \z
				' .. env.pathlist(env.incpath, '-I') .. '\z
				' .. env.pathlist(t.incpath, '-I') .. '\z
				-std=c++11 \z
				-pipe')
	end

	function env.link(t)
		return
			env.makepath(t.target) and
			env.execute('g++ \z
				-o "' .. env.path(t.target) .. '" \z
				' .. env.pathlist(t.items) .. '\z
				' .. env.pathlist(env.libpath, '-L') .. '\z
				' .. env.pathlist(t.libpath, '-L') .. '\z
				' .. env.pathlist(t.libs, '-l') .. '\z
				-pipe')
	end

	function env.lua(t)
		local varstr = {}
		if t.vars then
			for name, value in pairs(t.vars) do
				if type(value) == 'string' then
					table.append(varstr, name .. '=[[' .. value .. ']]')
				else
					table.append(varstr, name .. '=' .. value)
				end
			end
		end
		return env.execute('lua \z
			-e "' .. table.concat(varstr, ' ') .. '" \z
			"' .. env.path(t.script) .. '" \z
			' .. env.pathlist(t.args))
	end
end

env.incpath = '.'
env.libpath = '.'
