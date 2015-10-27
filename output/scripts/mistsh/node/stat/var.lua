local modname = ...
local node = require('mistsh.node')
local varstat = node:module(modname)
local common

function varstat:init(pr, context)
	node.init(self, pr, context)
	self.modifier = pr._modifier and common.createnode(pr._modifier, context)
	self.type = common.createnode(pr._type, context)
	self.name = pr._name
	self.value = pr._value and common.createnode(pr._value, context)
	if self.modifier then
		self.modifier:apply(self)
	end
end

function varstat:defstring(lp)
	local mstr
	if self.modifier then
		mstr = self.modifier:defstring(lp .. self.lpindent) .. ' '
	else
		mstr = ''
	end
	local vstr
	if self.value then
		vstr = ' = ' .. self.value:defstring(lp .. self.lpindent)
	else
		vstr = ''
	end
	return string.format('%s%s %s%s',
		mstr,
		self.type:defstring(lp .. self.lpindent),
		self.name,
		vstr)
end

function varstat:build(section)
	if self.modifier and self.modifier.buildvar then
		return self.modifier:buildvar(self, section)
	else
		local vstr
		if self.value then
			vstr = ' = ' .. self.value:build()
		else
			vstr = ''
		end
		return self.type:build() .. ' ' .. self.name .. vstr .. ';'
	end
end

common = require('mistsh.common')
