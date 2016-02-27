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

setfenv(1, _G)
protectglobaltable(true)

local info = require('host.info')
local indexmodule = info.configset:string('index', '')
if #indexmodule == 0 then
	error('index module required')
end

local isuc, indexmod = pcall(require, indexmodule)
local err
if isuc then
	isuc, err = pcall(indexmod.register)
else
	err = indexmod
end
if not isuc then
	print(err)
	require('host.window'):finish()
end
if info.configset:boolean('silent', false) then
	require('host.window'):finish()
end

local input = require('system.input')

return input.dispatch
