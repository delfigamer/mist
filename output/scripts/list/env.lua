local modname = ...
local object = require('base.object')
local env = object:module(modname)
local senamemap = require('host.sexprnamemap')
local secontext = require('host.sexprcontext')

local propertybits =
	{
		['flat'] = 1}

function env:init(heads, namemap, context)
	self.nameindex = {}
	self.namestring = {}
	self.namemap = namemap or senamemap:create()
	self.context = context or secontext:create()
	for i, entry in ipairs(heads) do
		local index = entry[1]
		local str = entry[2]
		local proplist = entry[3]
		self.nameindex[str] = index
		self.namestring[index] = str
		if not namemap then
			self.namemap:setname(index, str)
		end
		if proplist then
			local value = 0
			for i, prop in ipairs(proplist) do
				local bitv = propertybits[prop]
				if not bitv then
					error(string.format('unknown property %q', prop))
				end
				value = bit.bor(value, bitv)
			end
			self.context:setproperties(index, value)
		end
	end
end

function env:headindex(head)
	if not head then
		return 0
	elseif self.nameindex[head] then
		return self.nameindex[head]
	else
		error('invalid name string given')
	end
end

function env:headstring(index)
	if index == 0 then
		return nil
	elseif self.namestring[index] then
		return self.namestring[index]
	end
	local str = self.namemap:getname(index)
	if str then
		self.namestring[index] = str
		return str
	else
		error('invalid name index given')
	end
end

function env:getproperties(headindex)
	return self.context:getproperties(headindex)
end
