local modname = ...
local binaryexpr = require('mistsh.node.expr.binary')
local mulexpr = binaryexpr:module(modname)
mulexpr.optoken = '*'
