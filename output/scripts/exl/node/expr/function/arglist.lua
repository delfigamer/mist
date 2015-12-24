local modname = ...
local node = package.relrequire(modname, 3, 'base')
local farglist = node:module(modname)
local common
local context

function farglist:init(pr)
	node.init(self, pr)
	self.args = pr.args
end

function farglist:append(arg)
	table.append(self.args, arg)
end

function farglist:build(pc)
	for i, arg in ipairs(self.args) do
		arg:build(pc)
	end
end

function farglist:getargnames()
	local names = {}
	for i, arg in ipairs(self.args) do
		if arg.brvalue then
			table.append(names, arg.symbol.id.name)
		end
	end
	return names
end

function farglist:compilelocals(stream)
	for i, arg in ipairs(self.args) do
		arg:compilelocal(stream)
	end
end

function farglist:compilereturn(stream, resultarg)
	local names = {}
	if resultarg then
		table.append(names, resultarg.symbol:rcompile(stream))
	end
	for i, arg in ipairs(self.args) do
		if arg.blvalue then
			table.append(names, arg.symbol:rcompile(stream))
		end
	end
	stream:writetoken('a_return', names)
end

function farglist:defstring(lp)
	local arglines = {}
	for i, arg in ipairs(self.args) do
		arglines[i] = arg:defstring(lp)
	end
	return string.format('(%s)', table.concat(arglines, ', '))
end

common = package.relrequire(modname, 4, 'common')
context = package.relrequire(modname, 4, 'context')
