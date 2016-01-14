local modname = ...
local ebase = package.relrequire(modname, 1, 'base')
local ecast = ebase:module(modname)
local common
local fulltype

function ecast:init(pr)
	ebase.init(self, pr)
	self.base = pr.base
	self.typev = pr.typev
	self.blvalue = pr.blvalue
	self.brvalue = pr.brvalue
end

function ecast:dobuild(pc)
	self.base:build(pc)
	self.typev:build(pc)
	local baseft = self.base:getfulltype()
	local targetti = self.typev:gettivalue()
	if self.blvalue then
		if not baseft.lvalue then
			common.nodeerror(
				'source expression is not an lvalue',
				self)
		end
		self.lsource = common.createnode{
			name = 'expr.dummy',
			spos = self.spos,
			epos = self.epos,
			filename = self.filename,
			fulltype = fulltype:create(targetti, false, true),
		}
		self.lassignment = common.createnode{
			name = 'expr.operator',
			operator = 'assign',
			spos = self.spos,
			epos = self.epos,
			filename = self.filename,
			args = {
				self.base,
				self.lsource,
			},
		}
		self.lassignment:build(pc)
	end
	if self.brvalue then
		if not baseft.rvalue then
			common.nodeerror(
				'source expression is not an rvalue',
				self)
		end
		self.rtarget = common.createnode{
			name = 'expr.dummy',
			spos = self.spos,
			epos = self.epos,
			filename = self.filename,
			fulltype = fulltype:create(targetti, true, false),
		}
		self.rassignment = common.createnode{
			name = 'expr.operator',
			operator = 'assign',
			spos = self.spos,
			epos = self.epos,
			filename = self.filename,
			args = {
				self.rtarget,
				self.base,
			},
		}
		self.rassignment:build(pc)
	end
	self.fulltype = fulltype:create(targetti, self.blvalue, self.brvalue)
end

function ecast:lcompile(stream, source)
	if self.bsimplecast then
		self.base:lcompile(stream, source)
	else
		self.lsource:lcompile(stream, source)
		self.lassignment:rcompile(stream)
	end
end

function ecast:rcompile(stream)
	if not self.retname then
		if self.bsimplecast then
			self.retname = self.base:rcompile(stream)
		else
			local base = self.base:rcompile(stream)
			self.rassignment:rcompile(stream)
			self.retname = self.rtarget:rcompile(stream)
		end
	end
	return self.retname
end

function ecast:defstring(lp)
	local am
	if self.blvalue then
		if self.brvalue then
			am = 'inout '
		else
			am = 'out '
		end
	else
		if self.brvalue then
			am = ''
		else
			am = '<error> '
		end
	end
	return string.format('(%s as %s%s)',
		self.base:defstring(lp .. self.lpindent),
		am,
		self.typev:defstring(lp .. self.lpindent))
end

common = package.relrequire(modname, 3, 'common')
fulltype = package.relrequire(modname, 3, 'fulltype')
