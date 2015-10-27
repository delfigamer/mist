local modname = ...
return package.modulepack{
	prefix = modname .. '.',
	names = {
		'base',
		'function',
		'unit'}}
