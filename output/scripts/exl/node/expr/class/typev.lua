local modname = ...
local etypebase = require('exl.node.expr.typebase')
local eclasstypev = etypebase:module(modname)
local common
local classcontext
local classti

function eclasstypev:init(pr)
	etypebase.init(self, pr)
	self.parent = pr.parent
	self.body = pr.body
end

function eclasstypev:build(pc)
	if self.parent then
		self.parent:build(pc)
	end
-- inheritance is not implemented, so the parent value is ignored
	self.context = classcontext:create(nil, pc)
	self.typeinfo = classti:create{
		parent = self.parent,
		body = self.body,
	}
	self.body:build(pc)
end

function eclasstypev:defstring(lp)
	return classti.defstring(self, lp)
end

common = require('exl.common')
classcontext = require('exl.node.class.context')
classti = require('exl.node.class.ti')
