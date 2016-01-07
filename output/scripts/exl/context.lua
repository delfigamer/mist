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
	end
	self.lastid = 0
	self.opcontext = {}
end

function context:setsymbol(name, symbol)
	local sym = self.symbols[name]
	if sym then
		self.env:log('error',
			'symbol "'..name..'" is already defined at '..sym:getdefpos(),
			symbol:getdefpos())
		return
	end
	self.symbols[name] = symbol
end

function context:getsymbol(name)
	return
		self.symbols[name] or
		self.parent and self.parent:getsymbol(name)
end

function context:setop(op, proto, func)
	local set = self.opcontext[op]
	if not set then
		set = opset:create()
		self.opcontext[op] = set
	end
	local suc, err, spos, epos = set:insert(proto, func)
	if not suc then
		self.env:log('error', err, spos, epos)
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
