local modname = ...
return package.modulepack{
	prefix = modname .. '.',
	names = {
		'base',
		'const',
		'unit',
		'var'}}
