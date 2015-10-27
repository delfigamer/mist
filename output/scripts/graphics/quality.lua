local modname = ...
local quality = package.modtable(modname)

quality.gamma = 2.2
quality.invgamma = 1 / quality.gamma
quality.gammacorrection = true
