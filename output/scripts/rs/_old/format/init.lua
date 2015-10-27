local modname = ...
module(modname, package.seeall)

package.modulepack{
	target = _M,
	prefix = modname .. '.',
	names = {
		'base',
		'bitfont',
		'lua',
		'metadata',
		'png',
		'text'}}
