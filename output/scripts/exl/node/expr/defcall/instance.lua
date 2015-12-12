local modname = ...
local ebase = require('exl.node.expr.base')
local dinstance = ebase:module(modname)
local fulltype

function dinstance:init(pr)
	ebase.init(self, pr)
	self.context = pr.context
	self.base = pr.base
	self.outargs = pr.outargs
	self.inargs = pr.inargs
	if pr.ti then
		self.fulltype = fulltype:create(pr.ti, false, true)
	else
		self.fulltype = fulltype:create(nil, false, false)
	end
end

function dinstance:rcompile(stream)
	if self.retname == nil then
		local basename = self.base:rcompile(stream)
		local innames = {}
		for i, iarg in ipairs(self.inargs) do
			innames[i] = iarg:rcompile(stream)
		end
		local outnames = {}
		if self.fulltype.rvalue then
			self.retname = stream:genname()
			outnames[1] = self.retname
		else
			self.retname = false
		end
		for i, oarg in ipairs(self.outargs) do
			table.append(outnames, stream:genname())
		end
		stream:writetoken('a_call', basename, outnames, innames)
		for i, oarg in ipairs(self.outargs) do
			if self.retname then
				oarg:lcompile(stream, outnames[i+1])
			else
				oarg:lcompile(stream, outnames[i])
			end
		end
	end
	return self.retname
end

fulltype = require('exl.fulltype')
