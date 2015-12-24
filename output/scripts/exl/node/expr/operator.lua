local modname = ...
local ebase = package.relrequire(modname, 1, 'base')
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
		arg:build(pc)
	end
	local proto = {}
	for i, arg in ipairs(self.args) do
		proto[i] = arg:getfulltype()
	end
	local protostr = {}
	for i, arg in ipairs(proto) do
		table.append(protostr, arg:defstring(''))
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
				local candidates = {}
				for i, item in ipairs(itemlist) do
					candidates[i] = string.format(
						'%s-%s\t%s',
						item.operator.spos, item.operator.epos,
						item.operator)
				end
				pc.env:error(
					'error',
					'ambiguous ' .. protostr .. '\n' ..
						'possible candidates are:\n' ..
						table.concat(candidates, '\n'),
					self.spos, self.epos)
			end
		end
	end
	if not operatorfunc and proto[1] then
		operatorfunc = proto[1].ti:internalresolve(self.operator, proto)
	end
	if operatorfunc then
		local it = {
			context = pc,
			spos = self.spos,
			epos = self.epos,
			args = self.args,
		}
		self.operatorinstance = operatorfunc:createinstance(it)
	end
	if self.operatorinstance then
		self.constvalue = self.operatorinstance:getconstvalue()
		self.fulltype = self.operatorinstance:getfulltype()
	else
		pc.env:error(
			'cannot resolve ' .. protostr,
			self.spos, self.epos)
		return
	end
end

function eoperator:lcompile(stream, source)
	return self.operatorinstance:lcompile(stream, source)
end

function eoperator:rcompile(stream)
	if self.constvalue and self.constvalue.bsimplevalue then
		return self.constvalue:rcompile(stream)
	else
		local name = self.operatorinstance:rcompile(stream)
		local fts = self.fulltype:getserial()
		stream:writetoken('d_comment', fts or '-')
		return name
	end
end

function eoperator:defstring(lp)
	if binary[self.operator] and #self.args == 2 then
		return string.format('(%s %s %s)',
			self.args[1]:defstring(lp .. self.lpindent),
			binary[self.operator],
			self.args[2]:defstring(lp .. self.lpindent))
	elseif self.operator == 'call' and #self.args > 0 then
		local argstr = {}
		for i = 2, #self.args do
			argstr[i-1] = self.args[i]:defstring(lp .. self.lpindent)
		end
		return string.format('%s(%s)',
			self.args[1]:defstring(lp .. self.lpindent),
			table.concat(argstr, ', '))
	end
	local argstr = {}
	for i, arg in ipairs(self.args) do
		argstr[i] = arg:defstring(lp .. self.lpindent)
	end
	return string.format('operator %s(%s)',
		self.operator,
		table.concat(argstr, ', '))
end

common = package.relrequire(modname, 3, 'common')
