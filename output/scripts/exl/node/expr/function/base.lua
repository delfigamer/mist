local modname = ...
local ebase = require('exl.node.expr.base')
local efunctionbase = ebase:module(modname)
local common
local context
local exlbstream
local fulltype
local functionti

function efunctionbase:init(pr)
	ebase.init(self, pr)
	self.arglist = pr.arglist
	self.body = pr.body
	self.rettype = pr.rettype
	if self.rettype then
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

function efunctionbase:build(pc)
	self.context = context:create(pc)
	self.arglist:build(self.context)
	if self.resultarg then
		self.resultarg:build(self.context)
	end
	local ti = functionti:create{
		arglist = self.arglist,
		rettype = self.rettype,
	}
	self.fulltype = fulltype:create(ti, false, true)
	self.constvalue = self
end

function efunctionbase:rcompile(stream)
	if not self.retname then
		self.retname = stream:genname()
		local argnames = self.arglist:getargnames()
		stream:writetoken(
			'v_function', self.retname, self.context.depth, argnames)
		local bodystream = stream:substream()
		self.arglist:compilelocals(bodystream)
		if self.resultarg then
			self.resultarg:compilelocal(bodystream)
		end
		self.body:compile(bodystream)
		bodystream:writetoken('d_filepos', self.body.epos.row, self.body.epos.col)
		self.arglist:compilereturn(bodystream, self.resultarg)
		stream:writetoken('v_function_end')
	end
	return self.retname
end

common = require('exl.common')
context = require('exl.context')
exlbstream = require('exlb.stream')
fulltype = require('exl.fulltype')
functionti = require('exl.node.expr.function.ti')