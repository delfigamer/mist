local modname = ...
local object = require('base.object')
local namespace = object:module(modname)

function namespace:init(parent, env)
	self.parent = parent
	self.env = env
	self.symbols = {}
end

function namespace:set(name, symbol)
	local sym = self.symbols[name]
	if sym then
		self.env:log('error',
			'symbol "'..name'" is already defined at '..sym:getdefpos(),
			symbol:getdefpos())
		return
	end
	self.symbols[name] = symbol
end

function namespace:get(name)
	return
		self.symbols[name]
		or self.parent and self.parent:get(name)
end

function namespace:defstring(lp)
	local slines = {}
	for i, name, sym in table.spairs(self.symbols) do
		slines[i] = '\n' .. lp .. name .. ' = ' .. sym:defstring(lp)
	end
	return table.concat(slines)
end

function namespace.instmeta:__tostring()
	return self:defstring('')
end
