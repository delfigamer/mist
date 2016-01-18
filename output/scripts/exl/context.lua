local modname = ...
local object = package.relrequire(modname, 1, 'object')
local context = object:module(modname)
local common

function context:init(parent)
	self.parent = parent
	self.symbols = {}
	if parent then
		self.env = parent.env
		self.depth = parent.depth + 1
		self.nameprefix = parent.nameprefix
	else
		self.depth = 1
		self.nameprefix = ''
	end
	self.lastid = 0
	self.operators = {}
end

function context:setsymbol(name, symbol, node)
	local sym = self.symbols[name]
	if sym then
		common.nodeerror(
			string.format('symbol %s is already defined at %s:%s',
				common.identstring(name), sym.deffile, sym.defpos),
			node)
		return
	end
	self.symbols[name] = symbol
end

function context:getsymbol(name)
	return
		self.symbols[name] or
		self.parent and self.parent:getsymbol(name)
end

function context:getoperatorlist(op)
	local list = self.operators[op]
	if not list then
		list = {}
		self.operators[op] = list
	end
	return list
end

function context:getnameprefix()
	return self.nameprefix
end

function context:genid()
	self.lastid = self.lastid + 1
	return string.format('%s_%s',
		common.base26(self.depth), common.base26(self.lastid))
end

function context:defstring(lp)
	local slines = {}
	for i, name, sym in table.spairs(self.symbols) do
		slines[i] = '\n' .. lp .. name .. ' = ' .. sym:defstring(lp)
	end
	return table.concat(slines)
end

common = package.relrequire(modname, 1, 'common')
