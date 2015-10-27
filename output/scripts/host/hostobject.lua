local modname = ...
local ffipure = require('base.ffipure')
local ffi = require('ffi')
local hostobject = ffipure:module(modname)

hostobject.classtable = {}

function hostobject:derive(target, modname, classname, typename)
	target = ffipure.derive(self, target, modname)
	target.classname = classname
	target.typename = typename
	target.fields = string.format('%s* ptr;', typename)
	if classname then
		hostobject.classtable[classname] = target
	end
	return target
end

function hostobject:create()
	return nil
end
