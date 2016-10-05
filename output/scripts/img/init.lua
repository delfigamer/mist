local modname = ...

return package.modulepack{
	prefix = modname .. '.',
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
