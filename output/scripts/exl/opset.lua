local modname = ...
local object = package.relrequire(modname, 1, 'object')
local opset = object:module(modname)
local common

local function pcomp_exact(a, b)
	if #a ~= #b then
		return false
	end
	for i, aarg in ipairs(a) do
		local barg = b[i]
		if aarg.ti ~= barg.ti then
			return false
		elseif aarg.lvalue ~= barg.lvalue then
			return false
		elseif aarg.rvalue ~= barg.rvalue then
			return false
		end
	end
	return true
end

local function pcomp_permits(a, b)
	if #a ~= #b then
		return false
	end
	for i, aarg in ipairs(a) do
		local barg = b[i]
		if aarg.ti ~= barg.ti then
			if aarg.ti and barg.ti and not aarg.ti:iseq(barg.ti) then
				return false
			end
		elseif aarg.lvalue and not barg.lvalue then
			return false
		elseif aarg.rvalue and not barg.rvalue then
			return false
		end
	end
	return true
end

-- true means a is preferred over b
local function pcomp_prefer(a, b)
	for i, aarg in ipairs(a) do
		local barg = b[i]
		if barg.lvalue then
			if barg.rvalue then
				if not aarg.lvalue or not aarg.rvalue then
					return false
				end
			else
				if not aarg.lvalue and not aarg.rvalue then
					return false
				end
			end
		else
			if barg.rvalue then
				if not aarg.rvalue then
					return false
				end
			end
		end
	end
	return true
end

function opset:init()
	self.functions = {}
end

function opset:insert(prototype, operator)
	for i, item in ipairs(self.functions) do
		if pcomp_exact(item.prototype, prototype) then
			return false, 'duplicate operator'..operator, operator.spos
		end
	end
	table.append(self.functions, {
		prototype = prototype,
		operator = operator,
	})
	return true
end

function opset:resolve(prototype)
	local fcand = {}
	for i, item in ipairs(self.functions) do
		if pcomp_permits(item.prototype, prototype) then
			table.append(fcand, item)
		end
	end
	local scand = {}
	for i = 1, #fcand do
		local ip = fcand[i].prototype
		for j = 1, #fcand do
			local jp = fcand[j].prototype
			if i ~= j and pcomp_prefer(jp, ip) then
				goto next
			end
		end
		table.append(scand, fcand[i])
	::next::
	end
	return scand
end

function opset:defstring(lp)
	-- local slines = {}
	-- for i, item in table.spairs(self.list) do
		-- slines[i] = '\n' .. lp .. common.defstring(item.operator, lp)
	-- end
	-- return table.concat(slines)
	return 'opset'
end

common = package.relrequire(modname, 1, 'common')

--

local function pcomp_prefer_test()
	local ft = require('exl.fulltype')
-- when possible, [inout] variants are preferred
	print(pcomp_prefer(
		{ft:create(nil, true, true)},
		{ft:create(nil, true, true)}) == true)
	print(pcomp_prefer(
		{ft:create(nil, true, true)},
		{ft:create(nil, false, true)}) == true)
	print(pcomp_prefer(
		{ft:create(nil, true, true)},
		{ft:create(nil, true, false)}) == true)
	print(pcomp_prefer(
		{ft:create(nil, true, true)},
		{ft:create(nil, false, false)}) == true)
-- [in] variant is preferred over [out]
	print(pcomp_prefer(
		{ft:create(nil, false, true)},
		{ft:create(nil, true, true)}) == false)
	print(pcomp_prefer(
		{ft:create(nil, false, true)},
		{ft:create(nil, false, true)}) == true)
	print(pcomp_prefer(
		{ft:create(nil, false, true)},
		{ft:create(nil, true, false)}) == true)
	print(pcomp_prefer(
		{ft:create(nil, false, true)},
		{ft:create(nil, false, false)}) == true)
-- [out] variant
	print(pcomp_prefer(
		{ft:create(nil, true, false)},
		{ft:create(nil, true, true)}) == false)
	print(pcomp_prefer(
		{ft:create(nil, true, false)},
		{ft:create(nil, false, true)}) == false)
	print(pcomp_prefer(
		{ft:create(nil, true, false)},
		{ft:create(nil, true, false)}) == true)
	print(pcomp_prefer(
		{ft:create(nil, true, false)},
		{ft:create(nil, false, false)}) == true)
-- [N] variant is never preferred
	print(pcomp_prefer(
		{ft:create(nil, false, false)},
		{ft:create(nil, true, true)}) == false)
	print(pcomp_prefer(
		{ft:create(nil, false, false)},
		{ft:create(nil, false, true)}) == false)
	print(pcomp_prefer(
		{ft:create(nil, false, false)},
		{ft:create(nil, true, false)}) == false)
	print(pcomp_prefer(
		{ft:create(nil, false, false)},
		{ft:create(nil, false, false)}) == true)
	os.exit()
end
