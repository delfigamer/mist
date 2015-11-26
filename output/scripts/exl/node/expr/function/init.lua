local modname = ...
local ebase = require('exl.node.expr.base')
local efunction = ebase:module(modname)
local common
local context
local exlbstream

function efunction:init(pr)
	ebase.init(self, pr)
	self.arglist = pr.arglist
	self.body = pr.body
	if pr.rettype then
		self.resultarg = common.createnode{
			name = 'expr.function.arg',
			spos = self.arglist.epos,
			epos = self.arglist.epos,
			lvalue = true,
			rvalue = false,
			typev = pr.rettype,
			target = 'result',
		}
	end
end

function efunction:build(pc)
	self.context = context:create(pc)
	self.arglist:build(self.context)
	if self.resultarg then
		self.resultarg:build(self.context)
	end
	self.body:build(self.context)
end

function efunction:getconstvalue()
	return self
end

function efunction:rcompile(stream)
	local name = stream:genname()
	local argnames = self.arglist:getargnames()
	stream:writetoken('v_function', name, self.context.depth, argnames)
	local bodystream = stream:substream()
	self.arglist:compilelocals(bodystream)
	if self.resultarg then
		self.resultarg:compilelocal(bodystream)
	end
	self.body:compile(bodystream)
	self.arglist:compilereturn(bodystream, self.resultarg)
	stream:writetoken('v_function_end')
	return name
end

function efunction:defstring(lp)
	if self.rettype then
		return string.format('function%s: %s%s\n%send',
			common.defstring(self.arglist, lp .. self.lpindent),
			common.defstring(self.rettype, lp .. self.lpindent),
			common.defstring(self.body, lp .. self.lpindent),
			lp)
	else
		return string.format('function%s%s\n%send',
			common.defstring(self.arglist, lp .. self.lpindent),
			common.defstring(self.body, lp .. self.lpindent),
			lp)
	end
end

common = require('exl.common')
context = require('exl.context')
exlbstream = require('exlb.stream')
