local modname = ...
return package.modulepack{
	prefix = modname .. '.',
	names = {
		'farg',
		'farglist'}}
