local modname = ...
local object = require('base.object')
local utility
local stream
local rule = object:module(modname)

rule.name = '(rule)'
rule.scopeprefix = ''
rule.errorscope = false

function rule:new()
	local new = object.new(self)
	new.cache = setmetatable({}, {__mode="k"})
	return new
end

function rule:init(it)
	self.name = it.name
	self.scopeprefix = it.scopeprefix
end

function rule:cacheget(stream, row, col)
	return table.deepget(self.cache, stream, row, col)
end

function rule:cacheset(stream, row, col, value)
	table.deepset(self.cache, value, stream, row, col)
end

function rule:invoke(stream, row, col)
	row = row or 1
	col = col or 1
	local result = self:cacheget(stream, row, col)
	if result then
		return unpack(result)
	end
	local suc, er, ec, pr = xpcall(self.process,
		utility.operator.identity, self, stream, row, col)
	if not suc then
		if self.errorscope then
			error(string.format('[%i:%i] %s%s\n%s', row, col,
				self.scopeprefix, self.name, er), 0)
		else
			error(er, 0)
		end
	end
	if er then
		result = self:wrapresult(row, col, pr)
		if result == nil then
			result = false
		end
	else
		result = nil
	end
	self:cacheset(stream, row, col, {er, ec, result})
	return er, ec, result
end

function rule:wrapresult(row, col, pr)
	return pr
end

function rule:defstring()
	return self.name
end

function rule.instmeta:__tostring()
	return rawget(self, 'name') or self:defstring()
end

utility = require('base.utility')
stream = require('peg.stream')
