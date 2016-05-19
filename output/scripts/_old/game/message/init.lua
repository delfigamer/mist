local modname = ...
module(modname, package.seeall)

package.modulepack{
	target = _M,
	prefix = modname .. '.',
	names = {
		'base',
		'echo',	-- 1
		'kill',	-- 2
		'setlocation',	-- 3
	},
}

