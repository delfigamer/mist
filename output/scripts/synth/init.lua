local modname, target = ...

return package.modulepack{
	prefix = modname .. '.',
	target = target,
	names = {
		'common',
		'export',
		'filter',
		'func',
		'generator',
		'mixer',
		'modulator',
		'ramp',
	},
}
