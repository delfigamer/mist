local modname = ...

local function string_xgmatch(source, pattern)
	local items = {}
	source = string.gsub(source, pattern,
		function(...)
			table.append(items, table.pack(...))
			return ''
		end)
	return source, items
end

local function parse_meta(input)
	local result = {}
	for item in string.sub(input, 2, -2):gmatch('([^%s,][^,]*[^%s,])') do
		local name, value = string.match(item, '(.*%S)%s*=%s*(.*%S)')
		if name then
			result[name] = value
		else
			result[item] = true
		end
	end
	return result
end

local function parse_emit(meta, content)
	local ent = {
		type = 'emit',
		meta = parse_meta(meta),
		content = content,
	}
	ent.target = ent.meta.target or ''
	return ent
end

local function parse_enumbody(body)
	local items = {}
	for name, vstr in string.gmatch(
		body,
		'([%a_][%w_]*)%s*=%s*([xX%x]+)')
	do
		local value = tonumber(vstr)
		if value then
			table.append(items, {
				name = name,
				value = value,
			})
		end
	end
	return items
end

local function parse_enum(nsprefix, item)
	return {
		type = 'enum',
		meta = parse_meta(item[1]),
		keyword = 'enum ' .. item[2],
		nsprefix = nsprefix,
		cname = item[3],
		fcname = nsprefix .. item[3],
		items = parse_enumbody(item[4]),
	}
end

local function parse_parentstr(input)
	return string.match(input, '^:%s*public%s+(%S*)') or
		string.match(input, '^:%s*private%s+(%S*)') or
		string.match(input, '^:%s*(%S*)')
end

local function parse_class(nsprefix, item)
	return {
		type = 'class',
		meta = parse_meta(item[1]),
		keyword = item[2],
		nsprefix = nsprefix,
		cname = item[3],
		fcname = nsprefix .. item[3],
		parent = parse_parentstr(item[4]),
	},
	item[5]
end

local function parse_type(nsprefix, input)
	if input == 'void' then
		return {
			type = 'void',
		}
	end
	local base
	base = string.match(input, '^(.-)%s*%*$')
	if base then
		local baset = parse_type(nsprefix, base)
		return {
			type = 'pointer',
			base = baset,
		}
	end
	base = string.match(input, '^(.-)%s*const$') or
		string.match(input, '^const%s*(.-)$')
	if base then
		local baset = parse_type(nsprefix, base)
		baset.bconst = true
		return baset
	end
	return {
		type = 'reference',
		cname = input,
		nsprefix = nsprefix,
	}
end

local function parse_rettype(nsprefix, input)
	local rettype = string.match(input, 'static%s+(.*)')
	local bstatic = not not rettype
	rettype = parse_type(nsprefix, rettype or input)
	return bstatic, rettype
end

local function parse_attrs(input)
	local result = {}
	for item in string.gmatch(input, '(%S+)') do
		result[string.lower(item)] = true
	end
	return result
end

local function parse_arglist(nsprefix, input)
	local result = {}
	for item in string.sub(input, 2, -2):gmatch('([^%s,][^,]*[^%s,])') do
		local type, name = string.match(item, '^(.-)%s*([%a_][%w_]*)$')
		local arg = {
			type = parse_type(nsprefix, type),
			name = name,
		}
		table.append(result, arg)
	end
	return result
end

local function parse_method(nsprefix, class, item)
	local bstatic, rettype = parse_rettype(nsprefix, item[2])
	return {
		type = 'method',
		meta = parse_meta(item[1]),
		class = class,
		cname = item[3],
		bstatic = bstatic,
		rettype = rettype,
		arglist = parse_arglist(nsprefix, item[4]),
		attrs = parse_attrs(item[5]),
	}
end

local function parse_structfield(nsprefix, item)
	local type, name = string.match(item[1], '^(.-)%s*([%a_][%w_]*)$')
	return {
		type = parse_type(nsprefix, type),
		name = name,
	}
end

local function parse_struct(nsprefix, item)
	local body, fitems
	body = item[4]
	body, fitems = string_xgmatch(
		body,
		'%s*([^;{]*[^%s;{])%s*;')
	local fields = {}
	for i, fitem in ipairs(fitems) do
		table.append(fields, parse_structfield(nsprefix, fitem))
	end
	return {
		type = 'struct',
		meta = parse_meta(item[1]),
		keyword = item[2],
		nsprefix = nsprefix,
		cname = item[3],
		fcname = nsprefix .. item[3],
		fields = fields,
	}
end

local function parse_file(source)
	local entities = {}
	for meta, body in string.gmatch(
		source,
		'R_EMIT(%b())\z
			(.-)\z
			R_END%b()')
	do
		table.append(entities, parse_emit(meta, body))
	end
	source = string.gsub(
		source,
		'/%*.-%*/',
		'')
	source = string.gsub(
		source,
		'//[^\n]*',
		'')
	local namespaces = {{'::', source}}
	while #namespaces > 0 do
		local next = table.pop(namespaces)
		local nsprefix, source = next[1], next[2]
		local items

		source, items = string_xgmatch(
			source,
			'namespace%s+([%a_][%w_]*)%s*\z
				(%b{})')
		for i, item in ipairs(items) do
			local name, body = item[1], item[2]
			table.append(namespaces, {nsprefix .. name .. '::', body})
		end

		source, items = string_xgmatch(
			source,
			'R_ENUM(%b())%s*\z
				enum%s+([%a_][%w_]*)%s+([%a_][%w_]*)%s*\z
				(%b{})')
		for i, item in ipairs(items) do
			table.append(entities, parse_enum(nsprefix, item))
		end

		source, items = string_xgmatch(
			source,
			'R_STRUCT(%b())%s*\z
				([%a_][%w_]*)%s+([%a_][%w_]*)%s*\z
				(%b{})')
		for i, item in ipairs(items) do
			table.append(entities, parse_struct(nsprefix, item))
		end

		source, items = string_xgmatch(
			source,
			'R_CLASS(%b())%s*\z
				([%a_][%w_]*)%s+([%a_][%w_]*)%s*([%s%w_,:]*)\z
				(%b{})')
		for i, item in ipairs(items) do
			local class, body = parse_class(nsprefix, item)
			table.append(entities, class)
			local citems
			body, citems = string_xgmatch(
				body,
				'R_METHOD(%b())%s*\z
					(.-)%s*\z
					([%a_][%w_]*)%s*\z
					(%b())%s*\z
					([^;{]*)')
			for j, citem in ipairs(citems) do
				table.append(entities, parse_method(nsprefix, class, citem))
			end
		end
	end
	return entities
end

package.modtable(modname, parse_file)
