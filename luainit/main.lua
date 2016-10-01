local sourceptr, infostructptr = ...
package.loaded['host.info'] = infostructptr

local ffi = require('ffi')

sourceptr = ffi.cast(
	'struct{\z
		char const* name;\z
		char const* data;\z
		int length;\z
	}*', sourceptr)

while true do
	if sourceptr[0].data == nil then
		break
	end
	local name = ffi.string(sourceptr[0].name)
	local length = sourceptr[0].length
	local source = ffi.string(sourceptr[0].data, length)
	local chunk = assert(load(source, name))
	assert(pcall(chunk))
	sourceptr = sourceptr + 1
end

if package.loaded['host.info'].renderer_module ~= nil then
	local sourceptr = ffi.cast(
		'struct{\z
			char const* name;\z
			char const* data;\z
			int length;\z
		}*', package.loaded['host.info'].renderer_module)
	while true do
		if sourceptr[0].data == nil then
			break
		end
		local name = ffi.string(sourceptr[0].name)
		local length = sourceptr[0].length
		local source = ffi.string(sourceptr[0].data, length)
		local chunk = assert(load(source, name))
		assert(pcall(chunk))
		sourceptr = sourceptr + 1
	end
end

setfenv(1, _G)
protectglobaltable(true)

local info = require('host.info')
local configset = require('host.mainconf')
local indexmodule
if info.silent then
	indexmodule = configset:string('index_silent')
end
if not indexmodule then
	indexmodule = configset:string('index')
end
if not indexmodule then
	error('index module required')
end

local isuc, err = pcall(require, indexmodule)
if not isuc then
	print(err)
end
