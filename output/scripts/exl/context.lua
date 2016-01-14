local modname = ...
local object = package.relrequire(modname, 1, 'object')
local context = object:module(modname)
local common
local opset

function context:init(parent)
	self.parent = parent
	self.symbols = {}
	if parent then
		self.env = parent.env
		self.depth = parent.depth + 1
		self.namespace = parent.namespace
	else
		self.depth = 1
		self.namespace = '_'
	end
	self.lastid = 0
	self.opcontext = {}
end

function context:setsymbol(name, symbol)
	local sym = self.symbols[name]
	if sym then
		common.exlerror(
			string.format('symbol %s is already defined at %s:%s',
				common.identstring(name), sym.deffile, sym.defpos),
			symbol.defpos, nil, symbol.deffile)
		return
	end
	self.symbols[name] = symbol
end

function context:getsymbol(name)
	return
		self.symbols[name] or
		self.parent and self.parent:getsymbol(name)
end

function context:setop(op, proto, func, node)
	local set = self.opcontext[op]
	if not set then
		set = opset:create()
		self.opcontext[op] = set
	end
	local suc, prev = set:insert(proto, func)
	if not suc then
		common.nodeerror(
			string.format('%s is already defined at %s:%s',
				prev:defstring(''), prev.deffile, prev.defpos),
			node)
	end
end

function context:getop(op, proto)
	local result
	local set = self.opcontext[op]
	if set then
		result = set:resolve(proto)
	end
	if not result and self.parent then
		result = self.parent:getop(op, proto)
	end
	return result
end

function context:getnamespace()
	return self.namespace
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
opset = package.relrequire(modname, 1, 'opset')
