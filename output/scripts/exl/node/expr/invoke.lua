local modname = ...
local ebase = require(modname, 1, 'base')
local einvoke = ebase:module(modname)
local common

local binary = {
	plus = '+',
	addto = '+=',
	assign = '=',
	join = '..',
	divide = '/',
	times = '*',
	subtract = '-',
	subtractfrom = '-=',
}

local unary = {
	plus = '+',
	minus = '-',
}

function einvoke:init(pr)
	ebase.init(self, pr)
	self.target = pr.target
	self.args = pr.args
	self.binternal = pr.binternal
end

local function candidate_consider(list, impl)
	local removed = {}
	for i = 1, #list do
		local pc = list[i]:prefcompare(impl)
		if pc > 0 then
			return
		elseif pc < 0 then
			removed[i] = true
		end
	end
	local ti = 1
	for i = 1, #list do
		if not removed[i] then
			list[ti] = list[i]
			ti = ti + 1
		end
	end
	for i = ti, #list do
		list[i] = nil
	end
	table.append(list, impl)
end

local function tryresolve(self, context, visitedcontexts, protostr)
	if not context or visitedcontexts[context] then
		return
	end
	visitedcontexts[context] = true
	local candidates = {}
	local oplist = context:getoperatorlist(self.opname)
	for i, operator in ipairs(oplist) do
		local impl = operator:invoke(self)
		if impl then
			candidate_consider(candidates, impl)
		end
	end
	if #candidates == 1 then
		return candidates[1]
	elseif #candidates > 1 then
		local candstr = {}
		for i, item in ipairs(candidates) do
			local operator = item:getoperator()
			candstr[i] = string.format(
				'%s:%s\t%s',
				operator.deffile, operator.defpos,
				operator:defstring(''))
		end
		common.nodeerror(
			'ambiguous ' .. protostr .. '\n' ..
				'possible candidates are:\n' ..
				table.concat(candstr, '\n'),
			self)
	end
end

function einvoke:dobuild(pc)
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
	protostr = string.format('%s[%s]',
		self.opname,
		table.concat(protostr, ', '))
	self.context = pc
	local visitedcontexts = {}
	self.invocation = tryresolve(self, pc, visitedcontexts, protostr)
	if self.invocation then
		goto invocationfound
	end
	for i, arg in ipairs(proto) do
		local context = arg.ti:getcontext()
		self.invocation = tryresolve(self, context, visitedcontexts, protostr)
		if self.invocation then
			goto invocationfound
		end
	end
	do
		local current = pc.parent
		while current do
			self.invocation = tryresolve(self, current, visitedcontexts, protostr)
			if self.invocation then
				goto invocationfound
			end
			current = current.parent
		end
	end
	do
		if self.binternal then
			self.bfailed = true
		else
			common.nodeerror(
				'cannot resolve ' .. protostr,
				self)
		end
		return
	end
::invocationfound::
	self.constvalue = self.invocation:getconstvalue()
	self.fulltype = self.invocation:getfulltype()
end

function einvoke:lcompile(stream, source)
	return self.invocation:lcompile(stream, source)
end

function einvoke:rcompile(stream)
	if self.constvalue and self.constvalue.bsimplevalue then
		return self.constvalue:rcompile(stream)
	else
		local name = self.invocation:rcompile(stream)
		return name
	end
end

function einvoke:prefcompare(other)
	if not other['#'..modname] then
		return 0
	end
	return self.invocation:prefcompare(other.invocation)
end

function einvoke:defstring(lp)
	if binary[self.target] and #self.args == 2 then
		return string.format('(%s %s %s)',
			self.args[1]:defstring(lp .. self.lpindent),
			binary[self.target],
			self.args[2]:defstring(lp .. self.lpindent))
	elseif unary[self.target] and #self.args == 1 then
		return string.format('(%s %s)',
			unary[self.target],
			self.args[1]:defstring(lp .. self.lpindent))
	elseif self.target == 'invoke' and #self.args > 0 then
		-- check if base is a single identifier
		local basestr = self.args[1]:defstring(lp .. self.lpindent)
		if not string.match(basestr, '^[a-zA-Z_][a-zA-Z0-9]*$') then
			local argstr = {}
			for i = 2, #self.args do
				argstr[i-1] = self.args[i]:defstring(lp .. self.lpindent)
			end
			return string.format('%s[%s]',
				basestr,
				table.concat(argstr, ', '))
		end
	end
	local argstr = {}
	for i, arg in ipairs(self.args) do
		argstr[i] = arg:defstring(lp .. self.lpindent)
	end
	return string.format('%s[%s]',
		common.identstring(self.target),
		table.concat(argstr, ', '))
end

common = require(modname, 3, 'common')
