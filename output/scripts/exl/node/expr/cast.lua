local modname = ...
local ebase = package.relrequire(modname, 1, 'base')
local ecast = ebase:module(modname)
local common
local fulltype

function ecast:init(it)
	ebase.init(self, it)
	self.base = it.base
	self.fulltype = it.target
	self.context = it.context
	local baseft = self.base:getfulltype()
	local targetft = self.fulltype
	self.rank = 0
	if targetft.lvalue then
		self.lsource = common.createnode{
			name = 'expr.dummy',
			spos = self.spos,
			epos = self.epos,
			filename = self.filename,
			fulltype = fulltype:create(targetft.ti, false, true),
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
		local rank = self.lassignment.invocation:getrank()
		if rank > self.rank then
			self.rank = rank
		end
	end
	if targetft.rvalue then
		self.rtarget = common.createnode{
			name = 'expr.dummy',
			spos = self.spos,
			epos = self.epos,
			filename = self.filename,
			fulltype = fulltype:create(targetft.ti, true, false),
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
		local rank = self.rassignment.invocation:getrank()
		if rank > self.rank then
			self.rank = rank
		end
	end
end

function ecast:lcompile(stream, source)
	-- if self.bsimplecast then
		-- self.base:lcompile(stream, source)
	-- else
		self.lsource:lcompile(stream, source)
		self.lassignment:rcompile(stream)
	-- end
end

function ecast:rcompile(stream)
	if not self.retname then
		-- if self.bsimplecast then
			-- self.retname = self.base:rcompile(stream)
		-- else
			local base = self.base:rcompile(stream)
			self.rassignment:rcompile(stream)
			self.retname = self.rtarget:rcompile(stream)
		-- end
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
	local rlrank
	if it.binternal then
		rlrank = 0
	else
		if baseft.lvalue and not targetft.lvalue then
			rlrank = 1
		elseif baseft.rvalue and not targetft.rvalue then
			rlrank = 2
		else
			rlrank = 0
		end
	end
	if baseft.ti:iseq(targetft.ti) then
		-- log(baseft, targetft, rlrank)
		return it.base, rlrank
	end
	if not it.binternal then
		local expr = ecast:create(it)
		if not expr.bfailed then
			-- log(baseft, targetft, expr.rank)
			return expr, expr.rank + rlrank
		end
	end
end

common = package.relrequire(modname, 3, 'common')
fulltype = package.relrequire(modname, 3, 'fulltype')
