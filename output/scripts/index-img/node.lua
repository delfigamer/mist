local modname = ...
local object = require('base.object')
local node = object:module(modname)

function node:init(conf, input0, pintable)
	self.conf = conf
	self.input0 = input0
	self.pintable = pintable
end

function node:process()
end
