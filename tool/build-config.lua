local modname = ...
local ffi = require('ffi')

local configuration = {}

local platformset = {
	['win32'] = true,
	['win64'] = true,
}
if _G.platform then
	if platformset[_G.platform] then
		configuration.platform = _G.platform
	else
		error('unknown platform')
	end
else
	-- configuration.platform = ({
		-- ['Windows-x86'] = 'win32',
		-- ['Windows-x64'] = 'win64',
	-- })[ffi.os .. '-' .. ffi.arch] or error('unknown platform')
	configuration.platform = 'win64'
end

if _G.profile == 'release' then
	configuration.tag = 'release'
	configuration.debug = false
	configuration.saveasm = false
	configuration.compilermacros = {
	}
else
	configuration.tag = 'debug'
	configuration.debug = true
	configuration.saveintermediates = true
	configuration.compilermacros = {
		MIST_DEBUG = true,
	}
end

configuration.srcdir = 'src'
configuration.libincludedir = 'lib/include'
configuration.libstaticdir = 'lib/static'
configuration.libdynamicdir = 'lib/dynamic'
configuration.builddir =
	'build/l-' .. configuration.platform .. '-' .. configuration.tag
configuration.outputdir =
	'output/bin-l-' .. configuration.platform .. '-' .. configuration.tag

package.modtable(modname, configuration)
