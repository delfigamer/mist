local modname = ...
local object = require('base.object')
local context = object:module(modname)
local id
local opset

function context:init(parent)
	self.parent = parent
	self.symbols = {}
	if parent then
		self.env = parent.env
		self.depth = parent.depth + 1
		self.opset = opset:create(parent.opset)
	else
		self.depth = 1
		self.opset = opset:create()
	end
	self.lastid = 0
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
		self.symbols[name]
		or self.parent and self.parent:getsymbol(name)
end

function context:getopset()
	return self.opset
end

function context:genid()
	self.lastid = self.lastid + 1
	return id:create(self.depth, self.lastid)
end

function context:defstring(lp)
	local slines = {}
	for i, name, sym in table.spairs(self.symbols) do
		slines[i] = '\n' .. lp .. name .. ' = ' .. sym:defstring(lp)
	end
	return table.concat(slines)
end

-- function context.instmeta:__tostring()
	-- return self:defstring('')
-- end

id = require('exl.id')
opset = require('exl.opset')
