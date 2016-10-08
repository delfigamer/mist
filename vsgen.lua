assert(loadfile('base.lua'))()
local env = require('env')
local sources = require('build-sources')
local pageenvbase = {
	ipairs = ipairs,
	path = env.path,
	print = print,
	sources = sources,
	string = string,
}

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

function pageenvbase.find(str, pattern)
	return string.find(str, pattern, 1, true)
end

local function gen_handler(env, str)
	local func = assert(load(str, 'generator', 't', env))
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

local function translate(source, target)
	local srctext = getfilecontents(source)
	local env = setmetatable({}, {__index = pageenvbase})
	local result = string.gsub(
		srctext,
		'<%?lua(.-)%?>',
		function(str) return gen_handler(env, str) end)
	setfilecontents(target, result)
end

translate(
	'vs/client-main/client-main-template.vcxproj',
	'vs/client-main/client-main.vcxproj')
translate(
	'vs/luac/luac-template.vcxproj',
	'vs/luac/luac.vcxproj')
translate(
	'vs/luainit-main/luainit-main-template.vcxproj',
	'vs/luainit-main/luainit-main.vcxproj')
translate(
	'vs/luainit-renderer-d3d9/luainit-renderer-d3d9-template.vcxproj',
	'vs/luainit-renderer-d3d9/luainit-renderer-d3d9.vcxproj')
translate(
	'vs/luainit-renderer-gles/luainit-renderer-gles-template.vcxproj',
	'vs/luainit-renderer-gles/luainit-renderer-gles.vcxproj')
translate(
	'vs/methodlist-main/methodlist-main-template.vcxproj',
	'vs/methodlist-main/methodlist-main.vcxproj')
translate(
	'vs/methodlist-main/build-template.bat',
	'vs/methodlist-main/build.bat')
translate(
	'vs/methodlist-renderer-d3d9/methodlist-renderer-d3d9-template.vcxproj',
	'vs/methodlist-renderer-d3d9/methodlist-renderer-d3d9.vcxproj')
translate(
	'vs/methodlist-renderer-d3d9/build-template.bat',
	'vs/methodlist-renderer-d3d9/build.bat')
translate(
	'vs/methodlist-renderer-gles/methodlist-renderer-gles-template.vcxproj',
	'vs/methodlist-renderer-gles/methodlist-renderer-gles.vcxproj')
translate(
	'vs/methodlist-renderer-gles/build-template.bat',
	'vs/methodlist-renderer-gles/build.bat')
translate(
	'vs/renderer-d3d9/renderer-d3d9-template.vcxproj',
	'vs/renderer-d3d9/renderer-d3d9.vcxproj')
translate(
	'vs/renderer-gles/renderer-gles-template.vcxproj',
	'vs/renderer-gles/renderer-gles.vcxproj')
