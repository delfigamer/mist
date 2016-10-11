local modname = ...
local iowrapper = require(modname, 1, 'iowrapper')
local fileio = iowrapper:module(modname)
local hostfileio = require('host.fileio')

local modetable = {
	['r'] = 0,
	['r+'] = 1,
	['w'] = 2,
	['w+'] = 3,
	['p+'] = 4,
}

function fileio:init(path, mode)
	assert_arg(1, path, 'string')
	iowrapper.init(self, hostfileio:create(path, modetable[mode] or mode))
end
