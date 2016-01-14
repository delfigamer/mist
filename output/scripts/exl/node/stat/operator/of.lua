local modname = ...
local baseof = package.relrequire(modname, 3, 'operator.base.factory')
local customof = baseof:module(modname)
local common
local functioncallof

function customof:init(pr)
	baseof.init(self, pr)
	self.operator = pr.operator
	self.args = pr.args
	self.rettype = pr.rettype
	self.symbol = pr.symbol
end

function customof:createinstance(it)
	local ref = common.createnode{
		name = 'expr.reference',
		context = it.context,
		spos = it.spos,
		epos = it.spos,
		filename = it.filename,
		target = self.symbol,
	}
	local dcargs = {ref}
	for i, arg in ipairs(it.args) do
		dcargs[i+1] = arg
	end
	return functioncallof:createinstance{
		context = it.context,
		spos = it.spos,
		epos = it.epos,
		filename = it.filename,
		args = dcargs,
	}
end

function customof:defstring(lp)
	local argstr = {}
	for i, arg in ipairs(self.args) do
		argstr[i] = arg:defstring(lp .. self.lpindent)
	end
	if self.rettype then
		return string.format('operator %s(%s): %s',
			common.identstring(self.operator),
			table.concat(argstr, ', '),
			self.rettype:defstring(lp .. self.lpindent))
	else
		return string.format('operator %s(%s)',
			common.identstring(self.operator),
			table.concat(argstr, ', '))
	end
end

common = package.relrequire(modname, 4, 'common')
functioncallof = package.relrequire(modname, 3, 'operator.functioncall.factory')
