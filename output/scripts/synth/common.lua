local modname = ...
local common = package.modtable(modname)

common.samplerate = 48000
common.subcycles = 1
common.period = 1/(common.samplerate*common.subcycles)
