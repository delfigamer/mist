local modname = ...
local node = require(modname, 2, 'base')
local soperator = node:module(modname)
local common
local opcustom
local fulltype
local symconst

function soperator:init(pr)
	node.init(self, pr)
	self.opname = pr.opname
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

function soperator:dobuild(pc)
	self.value:build(pc)
	self.symbol = symconst:create{
		context = pc,
		defpos = self.spos,
		deffile = self.filename,
		fulltype = fulltype:create(self.value:getfulltype().ti, false, true),
		constvalue = self.value,
	}
	local proto = {}
	for i, arg in ipairs(self.args) do
		proto[i] = fulltype:create(
			arg.typev:gettivalue(), arg.blvalue, arg.brvalue)
	end
	local retfulltype
	if self.rettype then
		retfulltype = fulltype:create(self.rettype:gettivalue(), false, true)
	end
	self.operator = opcustom:create{
		defpos = self.epos,
		deffile = self.filename,
		opname = self.opname,
		args = proto,
		retfulltype = retfulltype,
		userargs = self.args,
		symbol = self.symbol,
	}
	table.append(pc:getoperatorlist(self.opname), self.operator)
end

function soperator:compile(stream)
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

function soperator:defstring(lp)
	local argstr = {}
	for i, arg in ipairs(self.args) do
		argstr[i] = arg:defstring(lp .. self.lpindent)
	end
	if self.rettype then
		return string.format('operator %s(%s): %s%s\n%send',
			common.identstring(self.opname),
			table.concat(argstr, ', '),
			self.rettype:defstring(lp .. self.lpindent),
			self.body:defstring(lp .. self.lpindent),
			lp)
	else
		return string.format('operator %s(%s)%s\n%send',
			common.identstring(self.opname),
			table.concat(argstr, ', '),
			self.body:defstring(lp .. self.lpindent),
			lp)
	end
end

common = require(modname, 3, 'common')
opcustom = require(modname, 0, 'opcustom')
fulltype = require(modname, 3, 'fulltype')
symconst = require(modname, 3, 'symbol.const')
