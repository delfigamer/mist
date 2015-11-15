local modname = ...
local node = require('exl.node')
local evoid = node:module(modname)
local common

function evoid:init(pr)
	node.init(self, pr)
end

function evoid:defstring(lp)
	return 'void'
end

common = require('exl.common')
