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
		return {[0]='ssa', sname}
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

local nop_token = {[0]='ancillary'}

local function count_uses(context, stream)
	for i, token in ipairs(stream.trace) do
		local opinfo = stat.ops[token[0]]
		for i, arginfo in ipairs(opinfo.args) do
			local arg = token[i]
			if arginfo.role == 'input' then
				if arg[0] == 'ssa' and arg[1] ~= 0 then
					context:incssacount(arg[1])
				elseif arg[0] == 'member' then
					if arg[1][0] == 'ssa' and arg[1][1] ~= 0 then
						context:incssacount(arg[1][1])
					end
				elseif arg[0] == nil then
					for i = 1, #arg do
						local item = arg[i]
						if item[0] == 'ssa' and item[1] ~= 0 then
							context:incssacount(item[1])
						end
					end
				end
			elseif arginfo.role == 'output' then
				if arg[0] == 'member' then
					if arg[1][0] == 'ssa' and arg[1][0] ~= 0 then
						context:incssacount(arg[1][1])
					end
				end
			end
		end
	end
end

local function remove_nop(stream)
	local ti = 1
	for si = 1, #stream.trace do
		local token = stream.trace[si]
		if token[0] ~= 'ancillary' or #token > 0 then
			stream.trace[ti] = token
			ti = ti + 1
		end
	end
	for i = ti, #stream.trace do
		stream.trace[i] = nil
	end
end

local function compact(stream)
	local context = context:create()
	count_uses(context, stream)
	for tokenindex, token in ipairs(stream.trace) do
		local opinfo = stat.ops[token[0]]
		for argindex, arginfo in ipairs(opinfo.args) do
			local arg = token[argindex]
			if arginfo.role == 'input' then
				if arg[0] == 'ssa' and arg[1] ~= 0 then
					token[argindex] = context:get(arg[1])
				elseif arg[0] == 'member' then
					if arg[1][0] == 'ssa' and arg[1][1] ~= 0 then
						arg[1] = context:get(arg[1][1])
					end
				elseif arg[0] == nil then
					for i = 1, #arg do
						local item = arg[i]
						if item[0] == 'ssa' and item[1] ~= 0 then
							arg[i] = context:get(item[1])
						end
					end
				end
			elseif arginfo.role == 'output' then
				if arg[0] == 'member' then
					if arg[1][0] == 'ssa' and arg[1][1] ~= 0 then
						arg[1] = context:get(arg[1][1])
					end
				end
			end
			if token[0] == 'move' then
				local target = token[2]
				if target[0] == 'ssa'
					and context:getssacount(target[1]) <= 1
				then
					context:set(target[1], token[1])
					stream.trace[tokenindex] = nop_token
				end
			end
		end
	end
	context:clear()
	for tokenindex = #stream.trace, 1, -1 do
		local token = stream.trace[tokenindex]
		local opinfo = stat.ops[token[0]]
		for argindex, arginfo in ipairs(opinfo.args) do
			local arg = token[argindex]
			if arginfo.role == 'output' then
				if arg[0] == 'ssa' and arg[1] ~= 0 then
					token[argindex] = context:get(arg[1])
				elseif arg[0] == nil then
					for i = 1, #arg do
						local item = arg[i]
						if item[0] == 'ssa' and item[1] ~= 0 then
							arg[i] = context:get(item[1])
						end
					end
				end
			end
			if token[0] == 'move' then
				local source = token[1]
				if source[0] == 'ssa'
					and context:getssacount(source[1]) == 1
				then
					context:set(source[1], token[2])
					stream.trace[tokenindex] = nop_token
				end
			end
		end
	end
	remove_nop(stream)
end

package.modtable(modname, compact)
