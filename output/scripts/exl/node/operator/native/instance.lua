local modname = ...
local baseoi = package.relrequire(modname, 2, 'base.instance')
local nativeoi = baseoi:module(modname)

function nativeoi:init(pr)
	baseoi.init(self, pr)
	self.nofunc = pr.nofunc
	self.args = pr.args
	if self.nofunc.constfunc then
		local cargs = {}
		for i, arg in ipairs(self.args) do
			local cv = arg:getconstvalue()
			if not cv then
				goto cfail
			end
			cargs[i] = cv
		end
		self.constvalue = self.nofunc.constfunc(cargs)
	::cfail::
	end
	self.fulltype = self.nofunc.rettype
end

function nativeoi:rcompile(stream)
	if not self.retname then
		local tokenargs = {}
		for i, arg in ipairs(self.args) do
			tokenargs[i] = {'ssa', arg:rcompile(stream)}
		end
		self.retname = stream:genname()
		table.append(tokenargs, {'ssa', self.retname})
		stream:writetoken{
			op = self.nofunc.opcode,
			args = tokenargs,
		}
	end
	return self.retname
end
