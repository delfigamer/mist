local modname = ...
module(modname, package.seeall)

package.modulepack{
	target = _M,
	prefix = modname .. '.',
	names = {
		'actorclass',
		'base',
		'bitfont',
		'text',
		'texture'}}
