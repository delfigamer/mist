local modname = ...
local node = package.relrequire(modname, 2, 'base')
local slocal = node:module(modname)
local common
local fulltype
local symlocal

function slocal:init(pr)
	node.init(self, pr)
	self.typev = pr.typev
	self.targetname = pr.targetname
	self.value = pr.value
end

function slocal:dobuild(pc)
	self.typev:build(pc)
	local typeinfo = self.typev:gettivalue()
	if not typeinfo then
		common.nodeerror(
			'this value does not define a type',
			self.typev)
	end
	local ft = fulltype:create(typeinfo, true, true)
	self.symbol = symlocal:create{
		context = pc,
		defpos = self.epos,
		deffile = self.filename,
		fulltype = ft,
	}
	if self.targetname then
		pc:setsymbol(self.targetname, self.symbol, self)
	end
	self.itarget = common.createnode{
		name = 'expr.dummy',
		spos = self.spos,
		epos = self.epos,
		filename = self.filename,
		fulltype = fulltype:create(typeinfo, true, false),
	}
	if self.value then
		self.value:build(pc)
		self.initop = common.createnode{
			name = 'expr.operator',
			operator = 'init',
			spos = self.spos,
			epos = self.epos,
			filename = self.filename,
			args = {
				self.itarget,
				self.value,
			},
		}
	else
		self.initop = common.createnode{
			name = 'expr.operator',
			operator = 'init',
			spos = self.spos,
			epos = self.epos,
			filename = self.filename,
			args = {
				self.itarget,
			},
		}
	end
	self.initop:build(pc)
end

function slocal:compile(stream)
	self.initop:rcompile(stream)
	local value = self.itarget:rcompile(stream)
	stream:writetoken{
		op = 'local_create',
		args = {
			{'ssa', value}, -- value
			{'local', self.symbol.id}, -- id
		},
	}
end

function slocal:defstring(lp)
	if self.value then
		return string.format('local %s %s = %s',
			self.typev:defstring(lp .. self.lpindent),
			common.identstring(self.targetname, lp .. self.lpindent),
			self.value:defstring(lp .. self.lpindent))
	else
		return string.format('local %s %s',
			self.typev:defstring(lp .. self.lpindent),
			common.identstring(self.targetname, lp .. self.lpindent))
	end
end

common = package.relrequire(modname, 3, 'common')
fulltype = package.relrequire(modname, 3, 'fulltype')
symlocal = package.relrequire(modname, 3, 'symbol.local')
