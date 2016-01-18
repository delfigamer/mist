local modname = ...
local node = package.relrequire(modname, 2, 'base')
local sclass = node:module(modname)
local common
local eclasstypev
local fulltype
local symconst

function sclass:init(pr)
	node.init(self, pr)
	self.classname = pr.classname
	self.parent = pr.parent
	self.body = pr.body
	self.value = eclasstypev:create{
		spos = self.spos,
		epos = self.epos,
		filename = self.filename,
		classname = self.classname,
		parent = self.parent,
	}
end

function sclass:dobuild(pc)
	self.value:build(pc)
	self.symbol = symconst:create{
		context = pc,
		defpos = self.spos,
		fulltype = fulltype:create(self.value:getfulltype().ti, false, true),
		constvalue = self.value,
	}
	pc:setsymbol(self.classname, self.symbol, self)
	self.body:build(self.value.classinfo:getcontext())
end

function sclass:compile(stream)
	local valname = self.value:rcompile(stream)
	stream:writetoken{
		op = 'local_create',
		args = {
			{'ssa', valname}, -- value
			{'local', self.symbol.id}, -- id
		},
	}
	self.body:compile(stream)
end

function sclass:defstring(lp)
	if self.parent then
		return string.format('class %s: %s%s\n%send',
			common.identstring(self.classname),
			self.parent:defstring(lp .. self.lpindent),
			self.body:defstring(lp .. self.lpindent),
			lp)
	else
		return string.format('class %s%s\n%send',
			common.identstring(self.classname),
			self.body:defstring(lp .. self.lpindent),
			lp)
	end
end

common = package.relrequire(modname, 3, 'common')
eclasstypev = package.relrequire(modname, 2, 'class.typev')
fulltype = package.relrequire(modname, 3, 'fulltype')
symconst = package.relrequire(modname, 3, 'symbol.const')
