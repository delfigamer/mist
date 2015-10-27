local modname = ...
local point = require('exl.point.base')
local exlcontext
local unitent
local unitv
local unitp = point:module(modname, 'unit')

function unitp:init(pr, context)
	point.init(self, pr, context)
	self.name = pr._name
	self.value = unitv:create{
		context = context,
		name = pr._name,
		point = self}
	self.publiccontext = exlcontext:create{
		parent = context,
		environment = self.value.namespace,
		visibility = 'public',
		unit = self.value}
	self.privatecontext = exlcontext:create{
		parent = context,
		environment = self.value.namespace,
		visibility = 'private',
		unit = self.value}
	self.entity = unitent:create{
		context = self.publiccontext,
		name = self.name,
		value = self.value}
	self.public = point:create(pr._public, self.publiccontext)
	self.private = point:create(pr._private, self.privatecontext)
end

function unitp:defstring(lp)
	local nlp = lp .. self.tabchar
	return 'unit '..self.value.innername..'\
'..lp..'public'..self.public:defstring(nlp)..'\
'..lp..'private'..self.private:defstring(nlp)..'\
'..lp..'end'
end

function unitp:compile_luai(lp)
	local nlp = lp .. self.tabchar
	return lp..'local '..self.value.innername..' = {}\
'..lp..'--[[public]]'..self.public:compile_luai(nlp)..'\
'..lp..'--[[private]]'..self.private:compile_luai(nlp)
end

exlcontext = require('exl.context')
unitent = require('exl.entity.unit')
unitv = require('exl.value.unit')
