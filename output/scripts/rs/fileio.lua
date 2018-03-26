local modname = ...
local iowrapper = require(modname, 1, 'iowrapper')
local fileio = iowrapper:module(modname)
local hostfileio = require('host.fileio')
local fileopenmode = require('host.fileopenmode')

local modetable = {
	['r'] = fileopenmode.read,
	['r+'] = fileopenmode.update,
	['w'] = fileopenmode.create,
	['w+'] = fileopenmode.create,
	['p'] = fileopenmode.provide,
	['p+'] = fileopenmode.provide,
}

function fileio:init(path, mode)
	assert_arg(1, path, 'string')
	iowrapper.init(self, hostfileio:create(path, modetable[mode] or mode))
end
