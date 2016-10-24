local modname = ...
local invprototyped = require(modname, 2, 'prototyped.invocation')
local invnative = invprototyped:module(modname)

function invnative:init(it)
	invprototyped.init(self, it)
	if self.operator.constfunc then
		local cargs = {}
		for i, arg in ipairs(self.args) do
			local cv = arg:getconstvalue()
			if not cv then
				goto cfail
			end
			cargs[i] = cv
		end
		self.constvalue = self.operator.constfunc{
			spos = it.spos,
			epos = it.epos,
			filename = it.filename,
			args = cargs,
		}
	::cfail::
	end
end

function invnative:rcompile(stream)
	if not self.retname then
		local stat = {
			[0]=self.operator.opcode,
		}
		for i, arg in ipairs(self.args) do
			stat[i] = {[0]='ssa', arg:rcompile(stream)}
		end
		self.retname = stream:genname()
		table.append(stat, {[0]='ssa', self.retname})
		stream:append(stat)
	end
	return self.retname
end
