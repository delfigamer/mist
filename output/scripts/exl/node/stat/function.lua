local modname = ...
local node = package.relrequire(modname, 2, 'base')
local sfunction = node:module(modname)
local common
local efunctionbase
local fulltype
local functionti
local symconst

function sfunction:init(pr)
	node.init(self, pr)
	self.targetname = pr.targetname
	self.arglist = pr.arglist
	self.rettype = pr.rettype
	self.body = pr.body
	self.value = efunctionbase:create{
		spos = self.spos,
		epos = self.epos,
		filename = self.filename,
		arglist = self.arglist,
		rettype = self.rettype,
		body = self.body,
	}
end

function sfunction:build(pc)
	self.value:build(pc)
	self.symbol = symconst:create{
		context = pc,
		defpos = self.spos,
		fulltype = fulltype:create(self.value:getfulltype().ti, false, true),
		constvalue = self.value,
	}
	pc:setsymbol(self.targetname, self.symbol)
	self.value.body:build(self.value.context)
end

function sfunction:compile(stream)
	stream:writetoken('a_createl', self.symbol.id, 0)
	local valname = self.value:rcompile(stream)
	stream:writetoken('a_setl', self.symbol.id, valname)
end

function sfunction:defstring(lp)
	if self.rettype then
		return string.format('function %s%s: %s%s\n%send',
			common.identstring(self.targetname),
			self.arglist:defstring(lp .. self.lpindent),
			self.rettype:defstring(lp .. self.lpindent),
			self.body:defstring(lp .. self.lpindent),
			lp)
	else
		return string.format('function %s%s%s\n%send',
			common.identstring(self.targetname),
			self.arglist:defstring(lp .. self.lpindent),
			self.body:defstring(lp .. self.lpindent),
			lp)
	end
end

common = package.relrequire(modname, 3, 'common')
efunctionbase = package.relrequire(modname, 2, 'expr.function.base')
fulltype = package.relrequire(modname, 3, 'fulltype')
functionti = package.relrequire(modname, 2, 'expr.function.ti')
symconst = package.relrequire(modname, 3, 'symbol.const')
