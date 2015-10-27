local modname = ...
local object = require('base.object')
local syntax = require('exl.syntax')
local pointset
local point = object:module(modname)

point.tabchar = syntax.tabchar

function point:derive(target, name)
	target = object.derive(self, target)
	target.create = object.create
	if name then
		pointset[name] = target
	end
	return target
end

function point:create(pr, ...)
	local name = assert(type(pr) == 'table' and pr.name,
		'parsing result expected')
	local class = assert(pointset[name],
		string.format('unknown point %s', name))
	return class:create(pr, ...)
end

function point:init(pr, context)
	self.position = {
		row = pr.row,
		col = pr.col,
		str = string.format('[%i:%i]', pr.row, pr.col)}
	self.context = context
end

function point:defstring(lp)
	return '<code point>'
end

function point:compile_luai(lp)
	return '--[[code point]]'
end

function point.instmeta:__tostring()
	return self:defstring('')
end

pointset = require('exl.point.pointset')
