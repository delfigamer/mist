local modname = ...
module(modname, package.seeall)
local module = require('ppcdecomp.module')

function decompile(sourcedata, conf)
	local m = module:create(sourcedata, conf)
	m:buildcalltree()
	return m:defstring('')
end
