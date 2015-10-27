local modname = ...
return package.modulepack{
	prefix = modname .. '.',
	names = {
		'assignment',
		'base'}}
