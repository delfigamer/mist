local modname = ...
local node = require('mistsh.node')
local basemodifier = node:module(modname)
local common

function basemodifier:init(pr, context)
	node.init(self, pr, context)
end

function basemodifier:defstring(lp)
	return self.name
end

function basemodifier:apply(var)
	print(var)
	print('mod kek')
end

common = require('mistsh.common')

