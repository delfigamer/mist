local modname = ...
local point = require('exl.point')
local assignmentstat = point:module(modname)
local common

function assignmentstat:init(pr, context)
	self.target = common.createpoint(pr._target, context)
	self.source = common.createpoint(pr._source, context)
end

function assignmentstat:defstring(lp)
	return string.format('%s = %s',
		self.target:defstring(lp .. self.lpindent),
		self.source:defstring(lp .. self.lpindent))
end

common = require('exl.common')
