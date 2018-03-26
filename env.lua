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

local function null_func(...)
	return ...
end

function env.flaglist(list, prefix, func)
	prefix = prefix or ''
	func = func or null_func
	local str = {}
	if not list then
		return ''
	elseif type(list) == 'string' then
		for item in string.gmatch(list, '[^;]+') do
			table.append(str, '"' .. prefix .. func(item) .. '" ')
		end
	else
		for i, item in ipairs(list) do
			str[i] = '"' .. prefix .. func(item) .. '" '
		end
	end
	return table.concat(str)
end

if not _G.toolchain or _G.toolchain == 'gcc' then
	function env.compile(t)
		local macrostr = {}
		if t.macros then
			for name, value in pairs(t.macros) do
				if type(value) == 'string' then
					table.append(macrostr, '"-D' .. name .. '=' .. value .. '" ')
				elseif value then
					table.append(macrostr, '"-D' .. name .. '" ')
				end
			end
		end
		local copt =
			'-O2 \z
			-Wall \z
			-Werror \z
			-Wno-invalid-offsetof \z
			-Wno-attributes \z
			' .. env.flaglist(env.incpath, '-I', env.path) .. '\z
			' .. env.flaglist(t.incpath, '-I', env.path) .. '\z
			' .. table.concat(macrostr) .. '\z
			-std=c++11 \z
			-pipe'
		if t.asmfile then
			return
				env.makepath(t.target) and
				env.execute('g++ \z
					-o "' .. env.path(t.asmfile) .. '" \z
					"' .. env.path(t.source) .. '" \z
					-S \z
					' .. copt) and
				env.execute('g++ \z
					-x assembler \z
					-o "' .. env.path(t.target) .. '" \z
					"' .. env.path(t.asmfile) .. '" \z
					-c')
		else
			return
				env.makepath(t.target) and
				env.execute('g++ \z
					-o "' .. env.path(t.target) .. '" \z
					"' .. env.path(t.source) .. '" \z
					-c \z
					' .. copt)
		end
	end

	function env.link(t)
		return
			env.makepath(t.target) and
			env.execute('g++ \z
				-o "' .. env.path(t.target) .. '" \z
				' .. env.flaglist(t.items, '', env.path) .. '\z
				' .. env.flaglist(env.libpath, '-L', env.path) .. '\z
				' .. env.flaglist(t.libpath, '-L', env.path) .. '\z
				' .. env.flaglist(t.libs, '-l', env.path) .. '\z
				' .. (t.dll and '-shared ' or '') .. '\z
				-fvisibility=hidden \z
				-pipe')
	end

	function env.lua(t)
		local varstr = {}
		if t.vars then
			for name, value in pairs(t.vars) do
				if type(value) == 'string' then
					table.append(varstr, name .. '=[[' .. value .. ']]')
				else
					table.append(varstr, name .. '=' .. tostring(value))
				end
			end
		end
		return env.execute('lua \z
			-e "' .. table.concat(varstr, ' ') .. '" \z
			"' .. env.path(t.script) .. '" \z
			' .. env.flaglist(t.args))
	end
end

env.incpath = '.'
env.libpath = '.'
