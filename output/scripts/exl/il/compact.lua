local modname = ...
local object = require('base.object')
local stat = require(modname, 1, 'stat')

local context = object:derive(nil, modname .. ':context')

function context:init()
	self.cache = {}
	self.ssacount = {}
end

function context:set(sname, value)
	self.cache[sname] = value
end

function context:get(sname)
	if self.cache[sname] then
		return self.cache[sname]
	else
		return {'ssa', sname}
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

local nop_token = {'ancillary'}

local function count_uses(context, block)
	for i, token in ipairs(block.body) do
		local opinfo = stat.ops[token[1]]
		for i, arginfo in ipairs(opinfo.args) do
			local arg = token[i+1]
			if arginfo.role == 'input' then
				if arg[1] == 'ssa' and arg[2] ~= 0 then
					context:incssacount(arg[2])
				elseif arg[1] == 'member' then
					if arg[2][1] == 'ssa' and arg[2][2] ~= 0 then
						context:incssacount(arg[2][2])
					end
				elseif arg[1] == 'list' then
					for i = 2, #arg do
						local item = arg[i]
						if item[1] == 'ssa' and item[2] ~= 0 then
							context:incssacount(item[2])
						end
					end
				end
			elseif arginfo.role == 'output' then
				if arg[1] == 'member' then
					if arg[2][1] == 'ssa' and arg[2][2] ~= 0 then
						context:incssacount(arg[2][2])
					end
				end
			end
		end
	end
end

local function remove_nop(block)
	local ti = 1
	for si = 1, #block.body do
		local token = block.body[si]
		if token[1] ~= 'ancillary' or #token > 1 then
			block.body[ti] = token
			ti = ti + 1
		end
	end
	for i = ti, #block.body do
		block.body[i] = nil
	end
end

local function compact(block)
	local context = context:create()
	count_uses(context, block)
	for tokenindex, token in ipairs(block.body) do
		local opinfo = stat.ops[token[1]]
		for argindex, arginfo in ipairs(opinfo.args) do
			local arg = token[argindex+1]
			if arginfo.role == 'input' then
				if arg[1] == 'ssa' and arg[2] ~= 0 then
					token[argindex+1] = context:get(arg[2])
				elseif arg[1] == 'member' then
					if arg[2][1] == 'ssa' and arg[2][2] ~= 0 then
						arg[2] = context:get(arg[2])
					end
				elseif arg[1] == 'list' then
					for i = 2, #arg do
						local item = arg[i]
						if item[1] == 'ssa' and item[2] ~= 0 then
							arg[i] = context:get(item[2])
						end
					end
				end
			elseif arginfo.role == 'output' then
				if arg[1] == 'member' then
					if arg[2][1] == 'ssa' and arg[2][2] ~= 0 then
						arg[2] = context:get(arg[2])
					end
				end
			end
			if token[1] == 'move' then
				local target = token[3]
				if target[1] == 'ssa'
					and context:getssacount(target[2]) <= 1
				then
					context:set(target[2], token[2])
					block.body[tokenindex] = nop_token
				end
			end
		end
	end
	context:clear()
	for tokenindex = #block.body, 1, -1 do
		local token = block.body[tokenindex]
		local opinfo = stat.ops[token[1]]
		for argindex, arginfo in ipairs(opinfo.args) do
			local arg = token[argindex+1]
			if arginfo.role == 'output' then
				if arg[1] == 'ssa' and arg[2] ~= 0 then
					token[argindex+1] = context:get(arg[2])
				elseif arg[1] == 'list' then
					for i = 2, #arg do
						local item = arg[i]
						if item[1] == 'ssa' and item[2] ~= 0 then
							arg[i] = context:get(item[2])
						end
					end
				end
			end
			if token[1] == 'move' then
				local source = token[2]
				if source[1] == 'ssa'
					and context:getssacount(source.value) == 1
				then
					context:set(source[2], token[3])
					block.body[tokenindex] = nop_token
				end
			end
		end
	end
	remove_nop(block)
end

package.modtable(modname, compact)
