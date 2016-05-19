local modname = ...
local binaryexpr = require('mistsh.node.expr.binary')
local addexpr = binaryexpr:module(modname)
addexpr.optoken = '+'
