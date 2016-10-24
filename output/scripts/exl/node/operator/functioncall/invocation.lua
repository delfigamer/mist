local modname = ...
local invbase = require(modname, 2, 'base.invocation')
local invfunctioncall = invbase:module(modname)
local fulltype

function invfunctioncall:init(it)
	invbase.init(self, it)
	-- self.context = it.context
	self.base = it.base
	self.outargs = it.outargs
	self.inargs = it.inargs
	if it.ti then
		self.fulltype = fulltype:create(it.ti, false, true)
	else
		self.fulltype = fulltype:create(invbase:getfulltype().ti, false, false)
	end
	self.rank = it.rank
end

function invfunctioncall:rcompile(stream)
	if not self.retname then
		local basename = self.base:rcompile(stream)
		local args = {[0]='list'}
		for i, iarg in ipairs(self.inargs) do
			args[i] = {[0]='ssa', iarg:rcompile(stream)}
		end
		local results = {}
		if self.fulltype.rvalue then
			self.retname = stream:genname()
			results[1] = self.retname
		else
			self.retname = 0
		end
		for i, oarg in ipairs(self.outargs) do
			table.append(results, stream:genname())
		end
		local resultargs = {[0]='list'}
		for i, rarg in ipairs(results) do
			table.append(resultargs, {[0]='ssa', rarg})
		end
		stream:append{
			[0]='call',
			{[0]='ssa', basename}, -- function
			args, -- args
			resultargs, -- results
		}
		for i, oarg in ipairs(self.outargs) do
			if self.fulltype.rvalue then
				oarg:lcompile(stream, results[i+1])
			else
				oarg:lcompile(stream, results[i])
			end
		end
	end
	return self.retname
end

fulltype = require(modname, 4, 'fulltype')
