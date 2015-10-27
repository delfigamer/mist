local modname = ...
module(modname, package.seeall)
local peg = require('peg')

parser, namespace = peg.loadfile(_PATH .. 'scripts/exl/syntax.peg')

tabchar = '\t'
