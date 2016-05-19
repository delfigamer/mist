local modname = ...
local node = require('index-img.node')
local save = node:module(modname)
local fmat = require('index-img.fmat')

function save:process()
	local ifmat = assert(self.input0).result
	ifmat:write(self.conf.file or 'output.fmat')
	self.result = ifmat
end
