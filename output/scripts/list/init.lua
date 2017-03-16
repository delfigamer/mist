local modname = ...
local list = package.modtable(modname)
local ffi = require('ffi')
local methodlist = require('host.info').client_methodlist
local databuffer = require('host.databuffer')
local sexpr = require('host.sexpr')

local sexpr_type = ffi.typeof('struct sexpr*')

local function sexpr_new(head)
	return sexpr:new(head or 0x40000000)
end

local function sexpr_clone(source)
	return sexpr:clone(source)
end

local function sexpr_detach(source)
	return sexpr:detach(source)
end

local function sexpr_assign(s, source)
	return sexpr:assign(s, source)
end

local function sexpr_move(s, source)
	return sexpr:move(s, source)
end

local function sexpr_sethead(s, head)
	return sexpr:sethead(s, head)
end

local function sexpr_getlength(s)
	return sexpr:getlength(s)
end

local function sexpr_setlength(s, length)
	return sexpr:setlength(s, length)
end

local function sexpr_getelements(s)
	return sexpr:getelements(s)
end

local function sexpr_gettext(s)
	local db = sexpr:gettext(s)
	if db then
		return ffi.string(db:getdata(), db:getlength())
	else
		return ''
	end
end

local function sexpr_settext(s, text)
	if text ~= '' then
		local db = databuffer:create(#text, #text, text)
		sexpr:settext(s, db)
	else
		sexpr:settext(s, nil)
	end
end

local function sexpr_compare(a, b, namemap)
	return sexpr:compare(a, b, namemap)
end

local function sexpr_sort(s, namemap)
	return sexpr:sort(s, namemap)
end

local function sexpr_normalize(s, context)
	return sexpr:normalize(s, context)
end

local function sexpr_dump(s, namemap)
	return sexpr:dump(s, namemap)
end

local function sexpr_setnumber(s, number)
	s.number = number
end

local sexpr_iterstate = ffi.typeof[[
	struct {
		struct sexpr* elements;
		size_t length;
	}]]

local function sexpr_elements_next(state, i)
	if i == state.length then
		return
	else
		return i + 1, state.elements + i + 1
	end
end

local function sexpr_elements(s)
	local elements = sexpr_getelements(s)
	local length = sexpr_getlength(s)
	return sexpr_elements_next, sexpr_iterstate(elements, length), 0
end

local function issexpr(s)
	return type(s) == 'cdata' and ffi.istype(s, sexpr_type)
end

local function sexpr_set(s, it, env)
	if issexpr(it) then
		sexpr_assign(s, it)
	elseif type(it) == 'string' then
		sexpr_sethead(s, 0x70000000)
		sexpr_settext(s, it)
	elseif type(it) == 'number' then
		sexpr_sethead(s, 0x50000000)
		sexpr_setnumber(s, it)
	elseif type(it) == 'boolean' then
		sexpr_sethead(s, it and 0x40000001 or 0x40000000)
	elseif type(it) == 'nil' then
		sexpr_sethead(s, 0x40000000)
	elseif type(it) == 'table' then
		local head = env:headindex(it[0])
		local length = #it
		sexpr_sethead(s, head)
		sexpr_setlength(s, length)
		for i, elem in sexpr_elements(s) do
			sexpr_set(elem, it[i])
		end
	else
		error('cannot pack this expression')
	end
end

local function getproperties(it, env)
	local headindex
	if issexpr(it) and it.head < 0x40000000 then
		headindex = it.head
	elseif type(it) == 'table' then
		headindex = env:headindex(it[0])
	else
		return
	end
	return env:getproperties(headindex)
end

function list.isflat(it, env)
	local value = getproperties(it, env)
	if value then
		return bit.band(value, 1) ~= 0
	end
end

function list.head(it, env)
	if type(it) == 'table' then
		return it[0]
	elseif issexpr(it) and it.head < 0x40000000 then
		return env:headstring(it.head)
	end
end

function list.packforce(it, env)
	local s = sexpr_new()
	sexpr_set(s, it, env)
	return s
end

function list.pack_r(it, env)
	if type(it) == 'table' then
		local ret = {}
		local packlist = true
		for i, elem in ipairs(it) do
			local pelem, suc = list.pack_r(elem, env)
			ret[i] = pelem
			packlist = packlist and suc
		end
		if packlist then
			local head = env:headindex(it[0])
			local s = sexpr_new(head)
			local length = #it
			sexpr_setlength(s, length)
			for i, elem in sexpr_elements(s) do
				sexpr_move(elem, ret[i])
			end
			return s, true
		else
			ret[0] = it[0]
			return ret, false
		end
	elseif
		type(it) == 'nil' or
		type(it) == 'boolean' or
		type(it) == 'number' or
		type(it) == 'string'
	then
		return list.packforce(it, env), true
	elseif issexpr(it) then
		return sexpr_copy(it), true
	else
		return it, false
	end
end

function list.pack(it, env)
	local ret = list.pack_r(it, env)
	return ret
end

function list.isatom(it, env)
	if issexpr(it) then
		return it.head >= 0x40000000
	else
		return type(it) ~= 'table'
	end
end

function list.asatom(it, env)
	if type(it) == 'table' then
		return
	elseif issexpr(it) then
		if it.head == 0x70000000 then
			return sexpr_gettext(it)
		elseif it.head == 0x50000000 then
			return it.number
		elseif it.head == 0x40000001 then
			return true
		elseif it.head == 0x40000000 then
			return false
		else
			return
		end
	else
		return it
	end
end

function list.elements(it)
	if type(it) == 'table' then
		return ipairs(it)
	elseif issexpr(it) and it.head < 0x40000000 then
		return sexpr_elements(it)
	else
		error('given sexpr is not a list')
	end
end

function list.outputform_tablelist(it, lp, tab, env)
	local elemstr = {}
	local deep = false
	local attrstr = {}
	for i, elem in list.elements(it) do
		local vstr = list.outputform_elem(elem, lp .. tab, tab, env)
		elemstr[i] = vstr
		if not list.isatom(elem) then
			deep = true
		end
	end
	for key, value in pairs(it) do
		if key ~= 0 and not elemstr[key] then
			local vstr = list.outputform_elem(value, lp .. tab, tab, env)
			table.append(attrstr, {key, vstr})
			deep = true
		end
	end
	list.sort(attrstr)
	for i, attr in ipairs(attrstr) do
		local str = string.format('%s -> %s', attr[1], attr[2])
		table.append(elemstr, str)
	end
	if deep then
		if it[0] then
			return string.format('%s[\n%s%s]',
				it[0], lp..tab, table.concat(elemstr, ',\n'..lp..tab))
		else
			return string.format('{\n%s%s}',
				lp..tab, table.concat(elemstr, ',\n'..lp..tab))
		end
	else
		if it[0] then
			return string.format('%s[%s]',
				it[0], table.concat(elemstr, ', '))
		else
			return string.format('{%s}',
				table.concat(elemstr, ', '))
		end
	end
end

function list.outputform_sexprlist(it, lp, tab, env)
	if not issexpr(it) or it.head >= 0x40000000 then
		error('given sexpr is not a list')
	end
	local elemstr = {}
	local deep = false
	for i, elem in list.elements(it) do
		local vstr = list.outputform_elem(elem, lp .. tab, tab, env)
		elemstr[i] = vstr
		if elem.head < 0x40000000 then
			deep = true
		end
	end
	local ithead = env:headstring(it.head)
	if deep then
		if ithead then
			return string.format('%s[\n%s%s]',
				ithead, lp..tab, table.concat(elemstr, ',\n'..lp..tab))
		else
			return string.format('{\n%s%s}',
				lp..tab, table.concat(elemstr, ',\n'..lp..tab))
		end
	else
		if ithead then
			return string.format('%s[%s]',
				ithead, table.concat(elemstr, ', '))
		else
			return string.format('{%s}',
				table.concat(elemstr, ', '))
		end
	end
end

function list.outputform_elem(it, lp, tab, env)
	local itype = type(it)
	if itype == 'table' then
		return list.outputform_tablelist(it, lp, tab, env)
	elseif itype == 'string' then
		return string.format('%q', it)
	elseif issexpr(it) then
		if it.head == 0x70000000 then
			return string.format('%q', sexpr_gettext(it))
		elseif it.head == 0x50000000 then
			return tostring(it.number)
		elseif it.head == 0x40000001 then
			return 'true'
		elseif it.head == 0x40000000 then
			return 'false'
		elseif it.head < 0x40000000 then
			return list.outputform_sexprlist(it, lp, tab, env)
		else
			return tostring(it)
		end
	else
		return tostring(it)
	end
end

function list.outputform(it, env)
	return list.outputform_elem(it, '', '\t', env)
end

local typeorder =
	{
		['nil'] = 0x40000000,
		['boolean'] = 0x40000000,
		['number'] = 0x50000000,
		['string'] = 0x70000000}

function list.compare(a,b, env)
	if list.isatom(a) then
		if list.isatom(b) then
			local a = list.asatom(a)
			local b = list.asatom(b)
			local atype = type(a)
			local btype = type(b)
			if atype ~= btype then
				return (typeorder[atype] or 0) - (typeorder[btype] or 0)
			elseif atype == 'number' or atype == 'string' then
				return a < b and -1 or a > b and 1 or 0
			elseif atype == 'boolean' then
				return (not a and b) and -1 or (a and not b) and 1 or 0
			else
				return 0
			end
		else
			return 1
		end
	else
		if list.isatom(b) then
			return -1
		elseif type(a) == 'table' or type(b) == 'table' then
			local hd = list.compare(list.head(a), list.head(b))
			if hd ~= 0 then
				return hd
			end
			local aif, ais, aii = list.elements(a)
			local bif, bis, bii = list.elements(b)
			local aelem, belem
			while true do
				aii, aelem = aif(ais, aii)
				bii, belem = bif(bis, bii)
				if aii then
					if bii then
						local d = list.compare(aelem, belem, env)
						if d ~= 0 then
							return d
						end
					else
						return 1
					end
				else
					if bii then
						return -1
					else
						return 0
					end
				end
			end
		else
			return sexpr_compare(a, b, env.namemap)
		end
	end
end

function list.lt(a,b, env)
	return list.compare(a,b, env) < 0
end

function list.eq(a,b, env)
	return list.compare(a,b, env) == 0
end

function list.sort(it, env)
	if issexpr(it) and it.head < 0x40000000 then
		return sexpr_sort(it, env.namemap)
	elseif type(it) == 'table' then
		return table.sort(it, function(a,b) return list.lt(a,b, env) end)
	else
		error('given sexpr is not a list')
	end
end

function list.copy(it)
	if issexpr(it) then
		return sexpr_copy(it)
	elseif type(it) == 'table' then
		local ret = {[0]=it[0]}
		for i, v in ipairs(it) do
			ret[i] = v
		end
		return ret
	else
		return it
	end
end

function list.detach(it)
	if issexpr(it) then
		return sexpr_detach(it)
	else
		return it
	end
end

function list.normalize(it, env)
	if issexpr(it) and it.head < 0x40000000 then
		return sexpr_normalize(it, env.context)
	elseif type(it) == 'table' then
		local ret
		local isflat = list.isflat(it, env)
		ret = {[0]=it[0]}
		for i, item in ipairs(it) do
			if list.isatom(item) then
				table.append(ret, item)
			else
				local nitem = list.normalize(item, env)
				if isflat and list.head(nitem) == it[0] then
					for j, subitem in list.elements(nitem) do
						table.append(ret, list.detach(subitem))
					end
				else
					table.append(ret, nitem)
				end
			end
		end
		return ret
	else
		error('given sexpr is not a list')
	end
end
