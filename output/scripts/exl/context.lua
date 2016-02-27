local modname = ...
local object = require(modname, 1, 'object')
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
	local symbolstr = {}
	for i, name, sym in table.spairs(self.symbols) do
		symbolstr[i] = string.format('\n%s%s = %s',
			lp..self.lpindent, name, sym:defstring(lp))
	end
	symbolstr = table.concat(symbolstr)
	local opstr = {}
	for i, opname, list in table.spairs(self.operators) do
		if #list > 0 then
			local lstr = {}
			for j, operator in ipairs(list) do
				lstr[j] = string.format('\n%s%s',
					lp..self.lpindent..self.lpindent, operator:defstring(lp))
			end
			lstr = table.concat(lstr)
			table.append(opstr, string.format('\n%s%s:%s',
				lp..self.lpindent, opname, lstr))
		end
	end
	opstr = table.concat(opstr)
	local parentstr
	if self.parent then
		parentstr = string.format('\n%sparent: %s', lp, self.parent.nameprefix)
	else
		parentstr = ''
	end
	return string.format('%snameprefix: %s%s\n%ssymbols:%s\n%soperators:%s',
		lp, self.nameprefix, parentstr, lp, symbolstr, lp, opstr)
end

common = require(modname, 1, 'common')
