local modname = ...
local node = package.relrequire(modname, 2, 'base')
local soperator = node:module(modname)
local common
local customof
local fulltype
local symconst

function soperator:init(pr)
	node.init(self, pr)
	self.operator = pr.operator
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
		fulltype = fulltype:create(self.value:getfulltype().ti, false, true),
		constvalue = self.value,
	}
	self.opfactory = customof:create{
		defpos = self.epos,
		deffile = self.filename,
		operator = self.operator,
		args = self.args,
		rettype = self.rettype,
		symbol = self.symbol,
	}
	local proto = {}
	for i, arg in ipairs(self.args) do
		local argti = arg.typev:gettivalue()
		proto[i] = fulltype:create(argti, arg.blvalue, arg.brvalue)
	end
	pc:setop(self.operator, proto, self.opfactory, self)
end

function soperator:compile(stream)
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

function soperator:defstring(lp)
	local argstr = {}
	for i, arg in ipairs(self.args) do
		argstr[i] = arg:defstring(lp .. self.lpindent)
	end
	if self.rettype then
		return string.format('operator %s(%s): %s%s\n%send',
			common.identstring(self.operator),
			table.concat(argstr, ', '),
			self.rettype:defstring(lp .. self.lpindent),
			self.body:defstring(lp .. self.lpindent),
			lp)
	else
		return string.format('operator %s(%s)%s\n%send',
			common.identstring(self.operator),
			table.concat(argstr, ', '),
			self.body:defstring(lp .. self.lpindent),
			lp)
	end
end

common = package.relrequire(modname, 3, 'common')
customof = package.relrequire(modname, 0, 'of')
fulltype = package.relrequire(modname, 3, 'fulltype')
symconst = package.relrequire(modname, 3, 'symbol.const')
