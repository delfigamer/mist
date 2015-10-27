local modname = ...
return package.modulepack{
	prefix = modname .. '.',
	names = {
		'base',
		'identref',
		'int',
		'nil',
		'reference',
		'scope',
		'unit'}}
