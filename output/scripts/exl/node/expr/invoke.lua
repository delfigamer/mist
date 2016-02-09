local modname = ...
local ebase = package.relrequire(modname, 1, 'base')
local einvoke = ebase:module(modname)
local common

local binary = {
	add = '+',
	adda = '+=',
	assign = '=',
	concat = '..',
	div = '/',
	mul = '*',
	sub = '-',
	suba = '-=',
}

local unary = {
	identity = '+',
	negate = '-',
}

function einvoke:init(pr)
	ebase.init(self, pr)
	self.opname = pr.opname
	self.args = pr.args
	self.binternal = pr.binternal
end

local function tryresolve(self, context, visitedcontexts, protostr)
	if not context or visitedcontexts[context] then
		return
	end
	visitedcontexts[context] = true
	local candidates
	local rank
	local oplist = context:getoperatorlist(self.opname)
	for i, operator in ipairs(oplist) do
		local impl = operator:invoke(self)
		if impl then
			local irank = impl:getrank()
			-- log(self, i, operator, irank)
			if irank == rank then
				table.append(candidates, impl)
			elseif not rank or irank < rank then
				candidates = {impl}
				rank = irank
			end
		end
	end
	if candidates and #candidates == 1 then
		return candidates[1]
	elseif candidates and #candidates > 1 then
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
	local impl = tryresolve(self, context.parent, visitedcontexts, protostr)
	if impl then
		return impl
	else
		return tryresolve(self, context.outer, visitedcontexts, protostr)
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
	protostr = string.format('operator %s(%s)',
		self.opname,
		table.concat(protostr, ', '))
	self.context = pc
	local visitedcontexts = {}
	self.invocation = tryresolve(self, pc, visitedcontexts, protostr)
	if not self.invocation then
		for i, arg in ipairs(proto) do
			local context = arg.ti:getcontext()
			self.invocation = tryresolve(self, context, visitedcontexts, protostr)
			if self.invocation then
				break
			end
		end
	end
	if self.invocation then
		self.constvalue = self.invocation:getconstvalue()
		self.fulltype = self.invocation:getfulltype()
	else
		if self.binternal then
			self.bfailed = true
		else
			common.nodeerror(
				'cannot resolve ' .. protostr,
				self)
		end
	end
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

function einvoke:defstring(lp)
	if binary[self.opname] and #self.args == 2 then
		return string.format('(%s %s %s)',
			self.args[1]:defstring(lp .. self.lpindent),
			binary[self.opname],
			self.args[2]:defstring(lp .. self.lpindent))
	elseif unary[self.opname] and #self.args == 1 then
		return string.format('(%s %s)',
			unary[self.opname],
			self.args[1]:defstring(lp .. self.lpindent))
	elseif self.opname == 'call' and #self.args > 0 then
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
	return string.format('invoke %s(%s)',
		self.opname,
		table.concat(argstr, ', '))
end

common = package.relrequire(modname, 3, 'common')
