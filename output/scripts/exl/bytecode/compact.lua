local modname = ...
local object = require('base.object')
local token = require(modname, 1, 'token')

local context = object:derive(nil, modname .. ':context')

function context:init()
	self.cache = {}
	self.ssacount = {}
end

function context:set(slot, value)
	local sname = tostring(slot)
	if sname then
		self.cache[sname] = value
	end
end

function context:get(slot)
	local sname = tostring(slot)
	if sname and self.cache[sname] then
		return self.cache[sname]
	else
		return slot
	end
end

function context:clear()
	self.cache = {}
end

function context:incssacount(id)
	local count = self.ssacount[id]
	self.ssacount[id] = count and (count + 1) or 1
end

function context:decssacount(id)
	self.ssacount[id] = self.ssacount[id] - 1
end

function context:getssacount(id)
	return self.ssacount[id] or 0
end

local nop_token = token:create{
	op = 'ancillary',
	args = {},
}

local function count_uses(context, block)
	for i, token in ipairs(block.parts) do
		for i, arginfo in ipairs(token.opinfo.args) do
			local arg = token.args[i]
			if arginfo.role == 'input' then
				if arg.type == 'ssa' and arg.value ~= 0 then
					context:incssacount(arg.value)
				elseif arg.type == 'list' then
					for i, item in ipairs(arg.items) do
						if item.type == 'ssa' and item.value ~= 0 then
							context:incssacount(item.value)
						end
					end
				elseif arg.type == 'member' then
					if arg.base.type == 'ssa' and arg.base.value ~= 0 then
						context:incssacount(arg.base.value)
					end
				end
			elseif arginfo.role == 'output' then
				if arg.type == 'member' then
					if arg.base.type == 'ssa' and arg.base.value ~= 0 then
						context:incssacount(arg.base.value)
					end
				end
			end
		end
	end
end

local function remove_nop(block)
	local ti = 1
	for si = 1, #block.parts do
		local token = block.parts[si]
		if token.opinfo.name ~= 'ancillary' or #token.args ~= 0 then
			block.parts[ti] = token
			ti = ti + 1
		end
	end
	for i = ti, #block.parts do
		block.parts[i] = nil
	end
end

local function compact(block)
	-- do return end
	local context = context:create()
	count_uses(context, block)
	for tokenindex, token in ipairs(block.parts) do
		for argindex, arginfo in ipairs(token.opinfo.args) do
			local arg = token.args[argindex]
			if arginfo.role == 'input' then
				if arg.type == 'ssa' and arg.value ~= 0 then
					token.args[argindex] = context:get(arg)
				elseif arg.type == 'list' then
					for itemindex, item in ipairs(arg.items) do
						if item.type == 'ssa' and item.value ~= 0 then
							arg.items[itemindex] = context:get(item)
						end
					end
				elseif arg.type == 'member' then
					if arg.base.type == 'ssa' and arg.base.value ~= 0 then
						arg.base = context:get(arg.base)
					end
				end
			elseif arginfo.role == 'output' then
				if arg.type == 'member' then
					if arg.base.type == 'ssa' and arg.base.value ~= 0 then
						arg.base = context:get(arg.base)
					end
				end
			end
			if token.opinfo.name == 'move' then
				local target = token.args[2]
				if target.type == 'ssa'
					and context:getssacount(target.value) == 1
				then
					context:set(target, token.args[1])
					block.parts[tokenindex] = nop_token
				end
			end
		end
	end
	context:clear()
	for tokenindex = #block.parts, 1, -1 do
		local token = block.parts[tokenindex]
		for argindex, arginfo in ipairs(token.opinfo.args) do
			local arg = token.args[argindex]
			if arginfo.role == 'output' then
				if arg.type == 'ssa' and arg.value ~= 0 then
					token.args[argindex] = context:get(arg)
				elseif arg.type == 'list' then
					for itemindex, item in ipairs(arg.items) do
						if item.type == 'ssa' and item.value ~= 0 then
							arg.items[itemindex] = context:get(item)
						end
					end
				end
			end
			if token.opinfo.name == 'move' then
				local source = token.args[1]
				if source.type == 'ssa'
					and context:getssacount(source.value) == 1
				then
					context:set(source, token.args[2])
					block.parts[tokenindex] = nop_token
				end
			end
		end
	end
	remove_nop(block)
end

package.modtable(modname, compact)
