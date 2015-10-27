local modname = ...
local binaryexpr = require('mistsh.node.expr.binary')
local subexpr = binaryexpr:module(modname)
subexpr.optoken = '-'
