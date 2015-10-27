local modname = ...
return package.modulepack{
	prefix = modname .. '.',
	names = {
		'context',
		'entity',
		'errors',
		'namespace',
		'point',
		'syntax',
		'type',
		'value'}}
