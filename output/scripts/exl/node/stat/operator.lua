local modname = ...
local node = package.relrequire(modname, 2, 'base')
local soperator = node:module(modname)
local common
local functioncallof
local efunctionbase
local fulltype
local symconst

function soperator:init(pr)
	node.init(self, pr)
	self.operator = pr.operator
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

function soperator:build(pc)
	self.value:build(pc)
	self.symbol = symconst:create{
		context = pc,
		defpos = self.spos,
		fulltype = fulltype:create(self.value:getfulltype().ti, false, true),
		constvalue = self.value,
	}
	local proto = {}
	for i, arg in ipairs(self.arglist.args) do
		local argti = arg.typev:gettivalue()
		proto[i] = fulltype:create(argti, arg.blvalue, arg.brvalue)
	end
	pc:setop(self.operator, proto, self)
	self.value.body:build(self.value.context)
end

function soperator:compile(stream)
	stream:writetoken('a_createl', self.symbol.id, 0)
	local valname = self.value:rcompile(stream)
	stream:writetoken('a_setl', self.symbol.id, valname)
end

function soperator:createinstance(it)
	local ref = common.createnode{
		name = 'expr.reference',
		context = it.context,
		spos = it.spos,
		epos = it.spos,
		filename = it.filename,
		target = self.symbol,
	}
	local dcargs = {ref}
	for i, arg in ipairs(it.args) do
		dcargs[i+1] = arg
	end
	return functioncallof:createinstance{
		context = it.context,
		spos = it.spos,
		epos = it.epos,
		filename = it.filename,
		args = dcargs,
	}
end

function soperator:defstring(lp)
	if self.rettype then
		return string.format('operator %s%s: %s%s\n%send',
			common.identstring(self.operator),
			self.arglist:defstring(lp .. self.lpindent),
			self.rettype:defstring(lp .. self.lpindent),
			self.body:defstring(lp .. self.lpindent),
			lp)
	else
		return string.format('operator %s%s%s\n%send',
			common.identstring(self.operator),
			self.arglist:defstring(lp .. self.lpindent),
			self.body:defstring(lp .. self.lpindent),
			lp)
	end
end

function soperator.instmeta:__tostring()
	if self.rettype then
		return string.format('operator %s%s: %s',
			common.identstring(self.operator),
			self.arglist:defstring(self.lpindent),
			self.rettype:defstring(self.lpindent))
	else
		return string.format('operator %s%s',
			common.identstring(self.operator),
			self.arglist:defstring(self.lpindent))
	end
end

common = package.relrequire(modname, 3, 'common')
functioncallof = package.relrequire(modname, 2, 'operator.functioncall.factory')
efunctionbase = package.relrequire(modname, 2, 'expr.function.base')
fulltype = package.relrequire(modname, 3, 'fulltype')
symconst = package.relrequire(modname, 3, 'symbol.const')
