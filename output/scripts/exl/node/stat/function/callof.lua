local modname = ...
local baseof = package.relrequire(modname, 3, 'operator.base.factory')
local functorcallof = baseof:module(modname)
local common
local functioncallof

function functorcallof:init(it)
	baseof.init(self, it)
	self.targetname = it.targetname
	self.args = it.args
	self.rettype = it.rettype
	self.symbol = it.symbol
end

function functorcallof:createinstance(it)
	local args = {}
	args[1] = common.createnode{
		name = 'expr.reference',
		context = it.context,
		spos = it.spos,
		epos = it.spos,
		filename = it.filename,
		target = self.symbol,
	}
	for i = 2, #it.args do
		args[i] = it.args[i]
	end
	return functioncallof:createinstance{
		context = it.context,
		spos = it.spos,
		epos = it.epos,
		filename = it.filename,
		args = args,
	}
end

function functorcallof:defstring(lp)
	local argstr = {}
	for i, arg in ipairs(self.args) do
		argstr[i] = arg:defstring(lp .. self.lpindent)
	end
	if self.rettype then
		return string.format('function %s(%s): %s',
			common.identstring(self.targetname),
			table.concat(argstr, ', '),
			self.rettype:defstring(lp .. self.lpindent))
	else
		return string.format('function %s(%s)',
			common.identstring(self.targetname),
			table.concat(argstr, ', '))
	end
end

common = package.relrequire(modname, 4, 'common')
functioncallof = package.relrequire(modname, 3, 'operator.functioncall.factory')
