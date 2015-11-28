local modname = ...
local ebase = require('exl.node.expr.base')
local ecall = ebase:module(modname)
local common
local deffunc
-- local prototype

function ecall:init(pr)
	ebase.init(self, pr)
	self.base = pr.base
	self.args = pr.args
	self.args[0] = self.base
end

function ecall:build(pc)
	for i = 0, #self.args do
		local arg = self.args[i]
		if arg then
			arg:build(pc)
		end
	end
	local proto = {}
	for i = 0, #self.args do
		local arg = self.args[i]
		if not arg then
			return
		end
		local argft = arg:getfulltype()
		if not argft then
			return
		end
		proto[i+1] = argft
	end
	local opset = pc:getopset()
	local operatorfunc = opset:resolve(
		'call',
		proto)
	if not operatorfunc then
		operatorfunc = deffunc
	end
	if operatorfunc then
		local it = {
			context = pc,
			spos = self.spos,
			epos = self.epos,
		}
		for i = 0, #self.args do
			it[i+1] = self.args[i]
		end
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
			string.format('cannot resolve operator call(%s)',
				table.concat(fts, ', ')),
			self.spos, self.epos)
		return
	end
end

function ecall:lcompile(stream, source)
	if self.operatorinstance then
		return self.operatorinstance:lcompile(stream, source)
	end
end

function ecall:rcompile(stream)
	if self.constvalue and self.constvalue.bsimplevalue then
		return self.constvalue:rcompile(stream)
	elseif self.operatorinstance then
		return self.operatorinstance:rcompile(stream)
	end
end

function ecall:defstring(lp)
	local args = {}
	for i, arg in ipairs(self.args) do
		args[i] = common.defstring(arg, lp .. self.lpindent)
	end
	return string.format('%s(%s)',
		common.defstring(self.base, lp .. self.lpindent),
		table.concat(args, ', '))
end

common = require('exl.common')
deffunc = require('exl.node.expr.call.default.func')
-- prototype = require('exl.prototype')
