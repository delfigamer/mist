local modname = ...
local regex = package.modtable(modname)

function regex.ignore(list, context)
	return context
end

function regex.any(list, context)
	return list and context
end

local function atom(a)
	return type(a) ~= 'table'
end

function regex.atom(list, context)
	return atom(list) and context
end

function regex.number(list, context)
	return type(list) == 'number' and context
end

function regex.string(list, context)
	return type(list) == 'string' and context
end

function regex.boolean(list, context)
	return type(list) == 'boolean' and context
end

function regex.choice(...)
	local vars = table.pack(...)
	return function(list, context)
		for i, pattern in ipairs(vars) do
			if regex.match(list, pattern, context) then
				return context
			end
		end
		return false
	end
end

function regex.capture(name, pattern)
	pattern = pattern or regex.any
	return function(list, context)
		if regex.match(list, pattern, context) then
			context[name] = list
		else
			return false
		end
		return context
	end
end

function regex.invoke(name, pattern)
	pattern = pattern or regex.any
	return function(list, context)
		local vcontext = {}
		if regex.match(list, pattern, vcontext) then
			context[name] = vcontext
		else
			return false
		end
		return context
	end
end

function regex.match(list, pattern, context)
	context = context or {}
	if type(pattern) == 'function' then
		return pattern(list, context)
	elseif type(list) ~= type(pattern) then
		return false
	elseif atom(pattern) then
		return list == pattern and context
	elseif #list > #pattern then
		return false
	else
		for i = 1, #pattern do
			if not regex.match(list[i], pattern[i], context) then
				return false
			end
		end
		return context
	end
end
