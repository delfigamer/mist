local modname = ...
local ebase = require('exl.node.expr.base')
local eoperator = ebase:module(modname)
local common

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
	local protostr = {}
	for i, arg in ipairs(proto) do
		table.append(protostr, common.defstring(arg))
	end
	protostr = string.format('operator %s(%s)',
		self.operator,
		table.concat(protostr, ', '))
	local operatorfunc
	do
		local itemlist = pc:getop(
			self.operator,
			proto)
		if itemlist then
			if #itemlist == 1 then
				operatorfunc = itemlist[1].operator
			elseif #itemlist > 1 then
				pc.env:log(
					'error',
					'ambiguous ' .. protostr,
					self.spos, self.epos)
				pc.env:log(
					'note',
					'possible candidates are:',
					self.spos, self.epos)
				for i, item in ipairs(itemlist) do
					pc.env:log(
						'note',
						item.operator,
						item.operator.spos, item.operator.epos)
				end
			end
		end
	end
	if not operatorfunc and proto[1] and proto[1].ti then
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
		pc.env:log(
			'error',
			'cannot resolve ' .. protostr,
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
