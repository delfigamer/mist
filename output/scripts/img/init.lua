local modname, target = ...

return package.modulepack{
	prefix = modname .. '.',
	target = target,
	names = {
		'add',
		'addscaled',
		'bloomgen',
		'cleanscan',
		'convolute',
		'divide',
		'export',
		'fmat',
		'frame',
		'gausskernel',
		'generate',
		'import',
		'level',
		'load',
		'locallims',
		'save',
		'wrap',
	},
}
