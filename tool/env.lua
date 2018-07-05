local modname = ...
local env = package.modtable(modname)
local ffi = require('ffi')
local configuration = require('build-config')

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

if configuration.toolchain == 'gcc' then
	local includeopt =
		'-I"src" ' ..
		'-I"' .. env.path(configuration.libincludedir) .. '" ' ..
		'-I"." '
	local commonopt = '\z
		-pipe \z
		-flto \z
		-freg-struct-return \z
		-mthreads '
	local copt = '\z
		-Wall \z
		-Werror \z
		-Wno-invalid-offsetof \z
		-Wno-attributes \z
		-std=c++11 \z
		-fno-fat-lto-objects ' ..
		includeopt ..
		commonopt
	local lopt = '\z
		-static-libgcc \z
		-static-libstdc++ \z
		-static -lpthread \z
		-mwindows \z
		-fuse-linker-plugin ' ..
		'-L"' .. configuration.libstaticdir .. '" ' ..
		commonopt
	if configuration.debug then
		copt = '\z
			-Og \z
			-flto-odr-type-merging ' ..
			copt
		lopt = '' ..
			lopt
	else
		copt = '\z
			-Ofast ' ..
			copt
		lopt = '\z
			-Ofast ' ..
			lopt
	end

	function env.compile(t)
		local macrostr = {}
		if t.macros then
			for name, value in pairs(t.macros) do
				if type(value) == 'string' then
					table.append(macrostr, '-D"' .. name .. '=' .. value .. '" ')
				elseif value then
					table.append(macrostr, '-D"' .. name .. '" ')
				end
			end
		end
		if t.asmfile then
			return
				env.makepath(t.target) and
				env.execute('g++ -S ' ..
					copt ..
					table.concat(macrostr) ..
					'-o "' .. env.path(t.asmfile) .. '" ' ..
					'"' .. env.path(t.source) .. '"') and
				env.execute('g++ -c ' ..
					'-o "' .. env.path(t.target) .. '" ' ..
					'-x assembler "' .. env.path(t.asmfile) .. '"')
		else
			return
				env.makepath(t.target) and
				env.execute('g++ -c ' ..
					copt ..
					table.concat(macrostr) ..
					'-o "' .. env.path(t.target) .. '" ' ..
					'"' .. env.path(t.source) .. '"')
		end
	end

	function env.link(t)
		return
			env.makepath(t.target) and
			env.execute('g++ ' ..
				lopt ..
				(t.dll and '-shared ' or '') ..
				'-o "' .. env.path(t.target) .. '" ' ..
				env.flaglist(t.items, '', env.path) ..
				env.flaglist(t.libs, '-l', env.path))
	end
elseif configuration.toolchain == 'cl' then
	local includeopt =
		'/I"src" ' ..
		'/I"' .. env.path(configuration.libincludedir) .. '" ' ..
		'/I"." '
	local pdbopt =
		'/Fd"' .. env.path(configuration.builddir .. '/vc141.pdb') .. '" '
	local copt = '\z
			/nologo \z
			/Gd \z
			/EHsc \z
			/fp:precise \z
			/D "_CONSOLE" \z
			/D "_UNICODE" \z
			/D "UNICODE" \z
			/errorReport:queue \z
			/W4 \z
			/wd4100 \z
			/wd4101 \z
			/wd4244 \z
			/wd4267 \z
			/wd4611 \z
			/wd4624 \z
			/wd4702 \z
			/wd5030 \z
			/WX \z
			/Zc:forScope \z
			/Zc:inline \z
			/Zc:wchar_t \z
			/diagnostics:classic \z
			/c \z
			/std:c++latest ' ..
		includeopt ..
		pdbopt
	local lopt = '\z
		/MANIFEST \z
		/NXCOMPAT \z
		/DYNAMICBASE \z
		/SUBSYSTEM:WINDOWS \z
		/MANIFESTUAC:"level=\'asInvoker\' uiAccess=\'false\'" \z
		/ERRORREPORT:QUEUE \z
		/NOLOGO \z
		/TLBID:1 ' ..
		'/LIBPATH:"' .. configuration.libstaticdir .. '" '
	if configuration.debug then
		copt = '\z
			/GS \z
			/Gm \z
			/ZI \z
			/Od \z
			/Oy- \z
			/sdl \z
			/D "_DEBUG" \z
			/RTC1 \z
			/MTd ' ..
			copt
		lopt = '\z
			/NODEFAULTLIB:libcmt \z
			/DEBUG \z
			/INCREMENTAL ' ..
			lopt
	else
		copt = '\z
			/GL \z
			/Gm- \z
			/Gy \z
			/Zi \z
			/O2 \z
			/Oi \z
			/sdl- \z
			/MT ' ..
			copt
		lopt = '\z
			/INCREMENTAL:NO \z
			/LTCG:incremental \z
			/OPT:REF \z
			/OPT:ICF \z
			' ..
			lopt
	end

	function env.compile(t)
		local macrostr = {}
		if t.macros then
			for name, value in pairs(t.macros) do
				if type(value) == 'string' then
					table.append(macrostr,
						'/D "' .. name .. '=' .. value .. '" ')
				elseif value then
					table.append(macrostr,
						'/D "' .. name .. '" ')
				end
			end
		end
		if t.asmfile then
			return
				env.makepath(t.target) and
				env.execute('cl ' ..
					copt ..
					table.concat(macrostr) ..
					'/Fo"' .. env.path(t.target) .. '" ' ..
					'/Fa"' .. env.path(t.asmfile) .. '" ' ..
					'"' .. env.path(t.source) .. '"')
		else
			return
				env.makepath(t.target) and
				env.execute('cl ' ..
					copt ..
					table.concat(macrostr) ..
					'/Fo"' .. env.path(t.target) .. '" ' ..
					'"' .. env.path(t.source) .. '"')
		end
	end

	function env.link(t)
		local pdbtarget =
			(string.match(t.target, '(.*)%.') or t.target) .. '.pdb'
		local manifesttarget =
			configuration.builddir .. '/' ..
			string.match(t.target, '[^/]*$') .. '.intermediate.manifest'
		local libstr = {}
		for i, lib in ipairs(t.libs) do
			table.append(libstr, '"' .. env.path(lib) .. '.lib" ')
		end
		return
			env.makepath(t.target) and
			env.execute('link ' ..
				lopt ..
				(t.dll and '/DLL ' or '') ..
				'/OUT:"' .. env.path(t.target) .. '" ' ..
				'/PDB:"' .. env.path(pdbtarget) .. '" ' ..
				'/ManifestFile:"' .. env.path(manifesttarget) .. '" ' ..
				table.concat(libstr) ..
				env.flaglist(t.items, '', env.path))
	end
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
