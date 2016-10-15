local modname = ...
local node = require(modname, 2, 'base')
local sfunction = node:module(modname)
local common
local fulltype
local opfunctorcall
local opfunctorcast
local symconst
local symfunctor

function sfunction:init(pr)
	node.init(self, pr)
	self.targetname = pr.targetname
	self.args = pr.args
	self.rettype = pr.rettype
	self.body = pr.body
	self.value = common.createnode{
		name = 'expr.function',
		spos = self.spos,
		epos = self.epos,
		filename = self.filename,
		args = self.args,
		rettype = self.rettype,
		body = self.body,
	}
end

function sfunction:dobuild(pc)
	self.functor = pc:getsymbol(self.targetname)
	if self.functor and
		not self.functor['#'..symfunctor._NAME]
	then
		common.nodeerror('only function definitions can be overloaded', self)
	elseif not self.functor then
		self.functor = symfunctor:create{
			context = pc,
			defpos = self.spos,
			deffile = self.filename,
			name = pc:getnameprefix() .. self.targetname,
		}
		pc:setsymbol(self.targetname, self.functor, self)
	end
	self.value:build(pc)
	self.symbol = symconst:create{
		context = pc,
		defpos = self.spos,
		deffile = self.filename,
		fulltype = fulltype:create(self.value:getfulltype().ti, false, true),
		constvalue = self.value,
	}
	local proto = {self.functor:getfulltype()}
	for i, arg in ipairs(self.args) do
		proto[i+1] = fulltype:create(
			arg.typev:gettivalue(), arg.blvalue, arg.brvalue)
	end
	local retfulltype
	if self.rettype then
		retfulltype = fulltype:create(self.rettype:gettivalue(), false, true)
	end
	self.opcall = opfunctorcall:create{
		defpos = self.spos,
		deffile = self.filename,
		opname = 'call',
		args = proto,
		retfulltype = retfulltype,
		targetname = self.targetname,
		userargs = self.args,
		symbol = self.symbol,
	}
	self.opcast = opfunctorcast:create{
		defpos = self.spos,
		deffile = self.filename,
		opname = 'cast',
		args = {
			fulltype:create(self.value:getfulltype().ti, true, false),
			self.functor:getfulltype(),
		},
		symbol = self.symbol,
	}
	local functorcontext = self.functor:getfulltype().ti:getcontext()
	table.append(functorcontext:getoperatorlist('call'), self.opcall)
	table.append(functorcontext:getoperatorlist('cast'), self.opcast)
end

function sfunction:compile(stream)
	stream:append{
		'local_create',
		{'ssa', 0}, -- value
		{'local', self.symbol.id}, -- id
	}
	local valname = self.value:rcompile(stream)
	stream:append{
		'move',
		{'ssa', valname}, -- source
		{'local', self.symbol.id}, -- target
	}
end

function sfunction:defstring(lp)
	local argstr = {}
	for i, arg in ipairs(self.args) do
		argstr[i] = arg:defstring(lp .. self.lpindent)
	end
	if self.rettype then
		return string.format('function %s(%s): %s%s\n%send',
			common.identstring(self.targetname),
			table.concat(argstr, ', '),
			self.rettype:defstring(lp .. self.lpindent),
			self.body:defstring(lp .. self.lpindent),
			lp)
	else
		return string.format('function %s(%s)%s\n%send',
			common.identstring(self.targetname),
			table.concat(argstr, ', '),
			self.body:defstring(lp .. self.lpindent),
			lp)
	end
end

common = require(modname, 3, 'common')
fulltype = require(modname, 3, 'fulltype')
opfunctorcall = require(modname, 0, 'opcall')
opfunctorcast = require(modname, 0, 'opcast')
symconst = require(modname, 3, 'symbol.const')
symfunctor = require(modname, 0, 'symbol')
