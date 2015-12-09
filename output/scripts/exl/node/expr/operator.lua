local modname = ...
local ebase = require('exl.node.expr.base')
local eoperator = ebase:module(modname)
local common
-- local prototype

local binary = {
	concat = '..',

	mul = '*',
	div = '/',

	add = '+',
	sub = '-',

	assign = '=',
	adda = '+=',
	suba = '-=',
}

function eoperator:init(pr)
	ebase.init(self, pr)
	self.operator = pr.operator
	self.args = pr.args
end

function eoperator:build(pc)
	for i, arg in ipairs(self.args) do
		if arg then
			arg:build(pc)
		end
	end
	if not self.operator then
		return
	end
	local proto = {}
	for i, arg in ipairs(self.args) do
		local argft = arg:getfulltype()
		if not argft then
			return
		end
		proto[i] = argft
	end
	local operatorfunc = pc:getop(
		self.operator,
		proto)
	if not operatorfunc and proto[1] then
		operatorfunc = proto[1].ti:getdefaultopfunc(self.operator, proto)
	end
	if operatorfunc then
		local it = {
			context = pc,
			spos = self.spos,
			epos = self.epos,
			args = self.args,
		}
		self.operatorinstance =
			operatorfunc:createinstance(it)
	end
	if self.operatorinstance then
		self.constvalue = self.operatorinstance:getconstvalue()
		self.fulltype = self.operatorinstance:getfulltype()
	else
		local fts = {}
		for i, arg in ipairs(proto) do
			table.append(fts, common.defstring(arg))
		end
		pc.env:log(
			'error',
			string.format('cannot resolve operator %s(%s)',
				self.operator,
				table.concat(fts, ', ')),
			self.spos, self.epos)
		return
	end
end

function eoperator:lcompile(stream, source)
	if self.operatorinstance then
		return self.operatorinstance:lcompile(stream, source)
	end
end

function eoperator:rcompile(stream)
	if self.constvalue and self.constvalue.bsimplevalue then
		return self.constvalue:rcompile(stream)
	elseif self.operatorinstance then
		return self.operatorinstance:rcompile(stream)
	end
end

function eoperator:defstring(lp)
	if self.operator then
		if binary[self.operator] and #self.args == 2 then
			return string.format('(%s %s %s)',
				common.defstring(self.args[1], lp .. self.lpindent),
				binary[self.operator],
				common.defstring(self.args[2], lp .. self.lpindent))
		elseif self.operator == 'call' then
			local args = {}
			for i = 2, #self.args do
				args[i-1] = common.defstring(self.args[i], lp .. self.lpindent)
			end
			return string.format('%s(%s)',
				common.defstring(self.args[1], lp .. self.lpindent),
				table.concat(args, ', '))
		end
	end
	local args = {}
	for i, arg in ipairs(self.args) do
		args[i] = common.defstring(arg, lp .. self.lpindent)
	end
	return string.format('operator %s(%s)',
		common.defstring(self.operator, lp .. self.lpindent),
		table.concat(args, ', '))
end

common = require('exl.common')
-- prototype = require('exl.prototype')
