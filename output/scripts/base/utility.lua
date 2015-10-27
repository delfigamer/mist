local modname = ...
module(modname, package.seeall)
local conf = require('config.system')

operator = {}

function operator.identity(a)
	return a
end

function operator.add(a,b)
	return a + b
end

function operator.sub(a,b)
	return a - b
end

function operator.mul(a,b)
	return a * b
end

function operator.div(a,b)
	return a / b
end

function operator.mod(a,b)
	return a % b
end

function operator.pow(a,b)
	return a ^ b
end

function operator.unm(a)
	return - a
end

function operator.concat(a,b)
	return a .. b
end

function operator.len(a)
	return #a
end

function operator.eq(a,b)
	return a == b
end

function operator.neq(a,b)
	return a ~= b
end

function operator.lt(a,b)
	return a < b
end

function operator.gt(a,b)
	return a > b
end

function operator.le(a,b)
	return a <= b
end

function operator.ge(a,b)
	return a >= b
end

function operator.index(a,b)
	return a[b]
end

function operator.newindex(a,b,c)
	a[b] = c
end

function operator.call(a,...)
	return a(...)
end

function operator.land(a, b)
	return a and b
end

function operator.lor(a, b)
	return a or b
end

function operator.lnot(a)
	return not a
end

function dummy()
end

function retriever(value)
	return function()
		return value
	end
end

function method(obj, func)
	return function(...)
		return func(obj, ...)
	end
end

function pack(...)
	local argv, argc = {...}, select('#', ...)
	return function(a, b)
		if a == '#' then
			return argc
		else
			return unpack(argv, a or 1, b or argc)
		end
	end
end

retriever_meta = {}

function retriever_meta:__call()
	return self.value
end

function retriever_meta:__tostring()
	return string.format(
		'<utility.retriever(%s)>',
		self.value)
end

function retriever_t(value)
	return setmetatable(
		{value = value},
		retriever_meta)
end

method_meta = {}

function method_meta:__call(...)
	return self.func(self.obj, ...)
end

function method_meta.__eq(a, b)
	return a.obj == b.obj and a.func == b.func
end

function method_meta:__tostring()
	return string.format(
		'<utility.method(%s, %s)>',
		self.obj, self.func)
end

function method_t(obj, func)
	return setmetatable(
		{obj = obj, func = func},
		method_meta)
end

pack_meta = {}

function pack_meta:__len()
	return a.length
end

function pack_meta:__ipairs_iter(index)
	index = index + 1
	if index <= self.length then
		return index, self[index]
	end
end

function pack_meta:__ipairs()
	return pack_meta.__ipairs_iter, self, 0
end

function pack_meta:__call(a, b)
	return unpack(self, a or 1, b or self.length)
end

function pack_meta:__tostring()
	local lines = {}
	for i = 1, self.length do
		lines[i] = tostring(self[i])
	end
	return string.format(
		'<utility.pack(%s)>',
		table.concat(lines, ', '))
end

function pack_t(...)
	return setmetatable(
		{length = select('#', ...), ...},
		pack_meta)
end

function table_insert(target, generator, f, s, var)
	while true do
		local args = pack(f(s, var))
		var = args(1)
		if var == nil then break end
		local key, value = generator(args())
		target[key] = value
	end
	return target
end

function table(generator, f, s, var)
	return table_insert({}, generator, f, s, var)
end

function array_genproxy(generator, i, ...)
	return i, generator(i, ...)
end

function array_insert(target, generator, f, s, var)
	return table_insert(target, method(array_genproxy, generator), f, s, var)
end

function array(generator, f, s, var)
	return array_insert({}, generator, f, s, var)
end

function range_iter(s, i)
	i = i + s.step
	if i <= s.b then
		return i
	end
end

function range(a, b, step)
	return range_iter, {step = step or 1, b = b}, a - (step or 1)
end
