local modname = ...
local object = require('base.object')
local persistent = object:module(modname)
local crc32 = require('crc32')
local ffi = require('ffi')
local scalars = require('rs.scalars')

local function registerproperty(class, it)
	local pr = {}
	pr.name = tostring(it[1])
	pr.nameid = crc32(pr.name)
	pr.type = it[2]
	pr.default = it.default
	pr.transient = it.transient
	pr.optional = it.optional or pr.transient or pr.default ~= nil
	table.append(class.ptype.properties, pr)
	class.ptype.propertymap[pr.nameid] = pr
end

function persistent:derive(target, modname, properties)
	local child = object.derive(self, target, modname)
	local ptype = {
		properties = {},
		propertymap = {},
		typename = modname,
		typeid = crc32(modname),
		class = child,
		write = self.ptype.write,
		read = self.ptype.read,
	}
	child.ptype = ptype
	for i, property in ipairs(self.ptype.properties) do
		ptype.properties[i] = property
		ptype.propertymap[property.nameid] = property
	end
	for i, property in ipairs(properties) do
		registerproperty(child, property)
	end
	return child
end

persistent.ptype = {
	properties = {},
	propertymap = {},
	typeid = crc32(modname),
	class = persistent,
}

function persistent.ptype:write(object, stream)
	for i, property in ipairs(self.properties) do
		if not property.transient then
			local value = object[property.name]
			if
				value ~= nil
				and (
					not property.default
					or value ~= self.class[property.name]
				)
			then
				local typeid = property.type.typeid
				scalars.int32:write(property.nameid, stream)
				scalars.int32:write(typeid, stream)
				if scalars[typeid] then
					property.type:write(value, stream)
				else
					local substream = stream:writesubstream()
					local suc, ret = pcall(property.type.write, property.type, value, substream)
					substream:close()
					if not suc then
						error(ret)
					end
				end
			end
		end
	end
	scalars.int32:write(0, stream)
end

function persistent.ptype:read(stream)
	local instance = self.class:new()
	local propreads = {}
	while true do
		local nameid = scalars.int32:read(stream)
		if nameid == 0 then
			break
		end
		local typeid = scalars.int32:read(stream)
		local property = self.propertymap[nameid]
		if property and property.type.typeid == typeid then
			if propreads[nameid] then
				error('duplicate property')
			end
			local value
			if scalars[typeid] then
				value = property.type:read(stream)
			else
				local substream = stream:readsubstream()
				local suc, ret = pcall(property.type.read, property.type, substream)
				substream:close()
				if suc then
					value = ret
				else
					error(ret)
				end
			end
			if value then
				instance[property.name] = value
				propreads[nameid] = true
			end
		else
			local stype = scalars[typeid]
			if stype then
				stype:skip(stream)
			else
				stream:skipsubstream()
			end
		end
	end
	for i, property in ipairs(self.properties) do
		if not propreads[property.nameid] and not property.optional then
			error('missing property')
		end
	end
	instance:initload()
	return instance
end

function persistent:initload()
end
