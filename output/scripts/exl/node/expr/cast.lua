local modname = ...
local ebase = require(modname, 1, 'base')
local ecast = ebase:module(modname)
local common
local fulltype

function ecast:init(it)
	ebase.init(self, it)
	self.base = it.base
	self.fulltype = it.target
	self.context = it.context
	self.baseft = self.base:getfulltype()
	self.targetft = self.fulltype
	self.btrivial = self.baseft.ti:iseq(self.targetft.ti)
	if not self.btrivial then
		if it.binternal then
			self.bfailed = true
			return
		end
		if self.targetft.lvalue then
			self.lsource = common.createnode{
				name = 'expr.dummy',
				spos = self.spos,
				epos = self.epos,
				filename = self.filename,
				fulltype = fulltype:create(self.targetft.ti, false, true),
			}
			self.lassignment = common.createnode{
				name = 'expr.invoke',
				opname = 'cast',
				spos = self.spos,
				epos = self.epos,
				filename = self.filename,
				args = {
					self.base,
					self.lsource,
				},
				binternal = true,
			}
			self.lassignment:build(self.context)
			if self.lassignment.bfailed then
				self.bfailed = true
				return
			end
		end
		if self.targetft.rvalue then
			self.rtarget = common.createnode{
				name = 'expr.dummy',
				spos = self.spos,
				epos = self.epos,
				filename = self.filename,
				fulltype = fulltype:create(self.targetft.ti, true, false),
			}
			self.rassignment = common.createnode{
				name = 'expr.invoke',
				opname = 'cast',
				spos = self.spos,
				epos = self.epos,
				filename = self.filename,
				args = {
					self.rtarget,
					self.base,
				},
				binternal = true,
			}
			self.rassignment:build(self.context)
			if self.rassignment.bfailed then
				self.bfailed = true
				return
			end
		end
	end
end

function ecast:lcompile(stream, source)
	if self.btrivial then
		self.base:lcompile(stream, source)
	else
		self.lsource:lcompile(stream, source)
		self.lassignment:rcompile(stream)
	end
end

function ecast:rcompile(stream)
	if not self.retname then
		if self.btrivial then
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
	-- if self.bexplicit then
		-- local am
		-- if self.blvalue then
			-- if self.brvalue then
				-- am = 'inout '
			-- else
				-- am = 'out '
			-- end
		-- else
			-- if self.brvalue then
				-- am = ''
			-- else
				-- am = '<error> '
			-- end
		-- end
		-- return string.format('(%s as %s%s)',
			-- self.base:defstring(lp .. self.lpindent),
			-- am,
			-- self.typev:defstring(lp .. self.lpindent))
	-- else
		return self.base:defstring(lp)
	-- end
end

function ecast:castvalue(it)
	local baseft = it.base:getfulltype()
	local targetft = it.target
	if targetft.lvalue and not baseft.lvalue then
		return
	end
	if targetft.rvalue and not baseft.rvalue then
		return
	end
	local expr = ecast:create(it)
	if not expr.bfailed then
		return expr
	end
end

function ecast:prefcompare(other)
	if self.btrivial and not other.btrivial then
		return 1
	end
	if not self.btrivial and other.btrivial then
		return -1
	end
	if not self.btrivial and not other.btrivial then
		local lcomp = 0
		local rcomp = 0
		if self.lassignment and other.lassignment then
			lcomp = self.lassignment:prefcompare(other.lassignment)
		end
		if self.rassignment and other.rassignment then
			rcomp = self.rassignment:prefcompare(other.rassignment)
		end
		if lcomp < 0 and rcomp <= 0 or lcomp <= 0 and rcomp < 0 then
			return -1
		elseif lcomp > 0 and rcomp >= 0 or lcomp >= 0 and rcomp > 0 then
			return 1
		end
	end
	if self.targetft.rvalue and not other.targetft.rvalue then
		return 1
	end
	if not self.targetft.rvalue and other.targetft.rvalue then
		return -1
	end
	if self.targetft.lvalue and not other.targetft.lvalue then
		return 1
	end
	if not self.targetft.lvalue and other.targetft.lvalue then
		return -1
	end
	return 0
end

common = require(modname, 3, 'common')
fulltype = require(modname, 3, 'fulltype')
