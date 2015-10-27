local modname = ...
return package.modulepack{
	prefix = modname .. '.',
	names = {
		'base',
		'debug_typeof',
		'decint',
		'function',
		'identifier',
		'index',
		'nil',
		'scope',
		'type'}}
