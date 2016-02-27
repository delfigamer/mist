local modname = ...
local object = require(modname, 1, 'object')
local opset = object:module(modname)
local common

local function proto_getserial(a)
	local items = {}
	for i, aarg in ipairs(a) do
		items[i] = aarg.ti:getserial()
	end
	return table.concat(items, '_')
end

local function pamcomp_equal(a, b)
	for i, aarg in ipairs(a) do
		local barg = b[i]
		if aarg.lvalue ~= barg.lvalue or aarg.rvalue ~= barg.rvalue then
			return false
		end
	end
	return true
end

local function pamcomp_permits(a, b)
	for i, aarg in ipairs(a) do
		local barg = b[i]
		if
			(aarg.lvalue and not barg.lvalue) or
			(aarg.rvalue and not barg.rvalue)
		then
			return false
		end
	end
	return true
end

-- true means a is preferred over b
local function pamcomp_prefer(a, b)
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
	self.serialmap = {}
end

function opset:insert(prototype, operator)
	local serial = proto_getserial(prototype)
	local slist = self.serialmap[serial]
	if not slist then
		slist = {}
		self.serialmap[serial] = slist
	end
	for i, item in ipairs(slist) do
		if pamcomp_equal(item.prototype, prototype) then
			return false, item.operator
		end
	end
	table.append(slist, {
		prototype = prototype,
		operator = operator,
	})
	return true
end

function opset:resolve(prototype)
	local serial = proto_getserial(prototype)
	local slist = self.serialmap[serial]
	if slist then
		local fcand = {}
		for i, item in ipairs(slist) do
			if pamcomp_permits(item.prototype, prototype) then
				table.append(fcand, item)
			end
		end
		local scand = {}
		for i = 1, #fcand do
			local ip = fcand[i].prototype
			for j = 1, #fcand do
				local jp = fcand[j].prototype
				if i ~= j and pamcomp_prefer(jp, ip) then
					goto next
				end
			end
			table.append(scand, fcand[i])
		::next::
		end
		if #scand > 0 then
			return scand
		end
	end
end

function opset:defstring(lp)
	return 'opset'
end

common = require(modname, 1, 'common')

--

local function pcomp_prefer_test()
	local ft = require(modname, 1, 'common')
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
