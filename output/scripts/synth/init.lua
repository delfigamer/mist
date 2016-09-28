local modname, target = ...

return package.modulepack{
	prefix = modname .. '.',
	target = target,
	names = {
		'common',
		'controller',
		'export',
		'func',
		'mixer',
		'note',
		'wrapper',
	},
}
