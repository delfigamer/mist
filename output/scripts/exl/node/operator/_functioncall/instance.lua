local modname = ...
local baseoi = package.relrequire(modname, 2, 'base.instance')
local functioncalloi = baseoi:module(modname)
local fulltype

function functioncalloi:init(pr)
	baseoi.init(self, pr)
	self.context = pr.context
	self.base = pr.base
	self.outargs = pr.outargs
	self.inargs = pr.inargs
	if pr.ti then
		self.fulltype = fulltype:create(pr.ti, false, true)
	else
		self.fulltype = fulltype:create(baseoi:getfulltype().ti, false, false)
	end
end

function functioncalloi:rcompile(stream)
	if self.retname == nil then
		local basename = self.base:rcompile(stream)
		local args = {}
		for i, iarg in ipairs(self.inargs) do
			args[i] = {'ssa', iarg:rcompile(stream)}
		end
		local results = {}
		if self.fulltype.rvalue then
			self.retname = stream:genname()
			results[1] = self.retname
		else
			self.retname = false
		end
		for i, oarg in ipairs(self.outargs) do
			table.append(results, stream:genname())
		end
		local resultargs = {}
		for i, rarg in ipairs(results) do
			table.append(resultargs, {'ssa', rarg})
		end
		stream:writetoken{
			op = 'call',
			args = {
				{'ssa', basename}, -- function
				{'list', items = args}, -- args
				{'list', items = resultargs}, -- results
			},
		}
		for i, oarg in ipairs(self.outargs) do
			if self.retname then
				oarg:lcompile(stream, results[i+1])
			else
				oarg:lcompile(stream, results[i])
			end
		end
	end
	return self.retname
end

fulltype = package.relrequire(modname, 4, 'fulltype')
