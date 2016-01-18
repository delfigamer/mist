local modname = ...
local node = package.relrequire(modname, 2, 'base')
local sfunction = node:module(modname)
local common
-- local efunctionbase
local fulltype
local functorcallof
-- local functionti
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
	if self.functor and not self.functor['#'..modname..'.symbol'] then
		common.nodeerror('only function definitions can be overloaded', self)
	elseif not self.functor then
		self.functor = symfunctor:create{
			context = pc,
			defpos = self.spos,
			deffile = self.filename,
			name = pc:getnamespace() .. '.' .. self.targetname,
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
	self.opfactory = functorcallof:create{
		defpos = self.spos,
		deffile = self.filename,
		targetname = self.targetname,
		args = self.args,
		rettype = self.rettype,
		symbol = self.symbol,
	}
	local proto = {self.functor:getfulltype()}
	for i, arg in ipairs(self.args) do
		proto[i+1] = fulltype:create(
			arg.typev:gettivalue(), arg.blvalue, arg.brvalue)
	end
	pc:setop('call', proto, self.opfactory, self)
end

function sfunction:compile(stream)
	stream:writetoken{
		op = 'local_create',
		args = {
			{'ssa', 0}, -- value
			{'local', self.symbol.id}, -- id
		},
	}
	local valname = self.value:rcompile(stream)
	stream:writetoken{
		op = 'move',
		args = {
			{'ssa', valname}, -- source
			{'local', self.symbol.id}, -- target
		},
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

common = package.relrequire(modname, 3, 'common')
-- efunctionbase = package.relrequire(modname, 2, 'expr.function.base')
fulltype = package.relrequire(modname, 3, 'fulltype')
functorcallof = package.relrequire(modname, 0, 'callof')
-- functionti = package.relrequire(modname, 2, 'expr.function.ti')
symconst = package.relrequire(modname, 3, 'symbol.const')
symfunctor = package.relrequire(modname, 0, 'symbol')
