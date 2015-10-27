local modname = ...
local binaryexpr = require('mistsh.node.expr.binary')
local divexpr = binaryexpr:module(modname)
divexpr.optoken = '/'
