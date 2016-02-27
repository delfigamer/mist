local window, hostmethodlist, path, confstr = ...

local ds = (platform == 'win') and '\\' or '/'

do
	local baselibfunc, err = loadfile(
		path .. 'scripts' .. ds .. 'system' .. ds .. 'baselib.lua')
	if baselibfunc then
		baselibfunc(confstr)
	else
		error(err)
	end
end

-- do return function() end end

package.modtable('host.methodlistptr', hostmethodlist)
package.modtable('host.windowptr', window)
local host = require('host')
window = require('host.window')
protectglobaltable(true)

local input = require('system.input')

local main = require('main', _CONFTABLE)
do
	local ffi = require('ffi')
	local cmdline = ffi.string(require('host.info').cmdline)
	local func, cerr = loadstring(cmdline, '', 't', main)
	if func then
		local suc, err = pcall(func)
		if not suc then
			log('error in command-line script:', err)
		end
	else
		log('error in command-line script:', cerr)
	end
end
if main.trace then
	require('base.trace').enable()
end
local isuc, indexmod = pcall(require, main.index)
local err
if isuc then
	isuc, err = pcall(indexmod.register)
else
	err = indexmod
end
if not isuc then
	print(err)
	window:finish()
end
if main.silent then
	require('host.window'):finish()
end

return input.dispatch
