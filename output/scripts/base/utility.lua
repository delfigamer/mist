local modname = ...
local utility = package.modtable(modname)

utility.operator = {}

function utility.operator.identity(a)
	return a
end

function utility.operator.add(a,b)
	return a + b
end

function utility.operator.sub(a,b)
	return a - b
end

function utility.operator.mul(a,b)
	return a * b
end

function utility.operator.div(a,b)
	return a / b
end

function utility.operator.mod(a,b)
	return a % b
end

function utility.operator.pow(a,b)
	return a ^ b
end

function utility.operator.unm(a)
	return - a
end

function utility.operator.concat(a,b)
	return a .. b
end

function utility.operator.len(a)
	return #a
end

function utility.operator.eq(a,b)
	return a == b
end

function utility.operator.neq(a,b)
	return a ~= b
end

function utility.operator.lt(a,b)
	return a < b
end

function utility.operator.gt(a,b)
	return a > b
end

function utility.operator.le(a,b)
	return a <= b
end

function utility.operator.ge(a,b)
	return a >= b
end

function utility.operator.index(a,b)
	return a[b]
end

function utility.operator.newindex(a,b,c)
	a[b] = c
end

function utility.operator.call(a,...)
	return a(...)
end

function utility.operator.land(a, b)
	return a and b
end

function utility.operator.lor(a, b)
	return a or b
end

function utility.operator.lnot(a)
	return not a
end

function utility.dummy()
end

function utility.retriever(value)
	return function()
		return value
	end
end

function utility.bind(f, ...)
	local argc, argv = select('#', ...), {...}
	for i = 1, argc do
		local inf = f
		local bv = argv[i]
		f = function(...)
			return inf(bv, ...)
		end
	end
	return f
end

-- function utility.table_insert(target, generator, f, s, var)
	-- while true do
		-- local args = pack(f(s, var))
		-- var = args(1)
		-- if var == nil then break end
		-- local key, value = generator(args())
		-- target[key] = value
	-- end
	-- return target
-- end

-- function utility.table(generator, f, s, var)
	-- return table_insert({}, generator, f, s, var)
-- end

-- function utility.array_genproxy(generator, i, ...)
	-- return i, generator(i, ...)
-- end

-- function utility.array_insert(target, generator, f, s, var)
	-- return table_insert(target, method(array_genproxy, generator), f, s, var)
-- end

-- function utility.array(generator, f, s, var)
	-- return array_insert({}, generator, f, s, var)
-- end

-- function utility.range_iter(s, i)
	-- i = i + s.step
	-- if i <= s.b then
		-- return i
	-- end
-- end

-- function utility.range(a, b, step)
	-- return range_iter, {step = step or 1, b = b}, a - (step or 1)
-- end
