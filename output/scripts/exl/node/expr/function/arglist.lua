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

function farglist:getinargs()
	local inargs = {}
	for i, arg in ipairs(self.args) do
		if arg.brvalue then
			table.append(inargs, arg.symbol.id)
		end
	end
	return inargs
end

function farglist:getarglocals()
	local args = {}
	for i, arg in ipairs(self.args) do
		if not arg.brvalue and arg.blvalue then
			table.append(args, arg.symbol.id)
		end
	end
	return args
end

function farglist:getoutargs(stream)
	local outargs = {}
	for i, arg in ipairs(self.args) do
		if arg.blvalue then
			table.append(outargs, arg.symbol.id)
		end
	end
	return outargs
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
