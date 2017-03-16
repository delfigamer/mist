local modname = ...
local list = package.modtable(modname)

local lua_keywords = table.makeset{
	'and',
	'break',
	'do',
	'else',
	'elseif',
	'end',
	'false',
	'for',
	'function',
	'goto',
	'if',
	'in',
	'local',
	'nil',
	'not',
	'or',
	'repeat',
	'return',
	'then',
	'true',
	'until',
	'while'}

local function inputform_keystring(key)
	if type(key) == 'string' then
		if lua_keywords[key] then
			return string.format('[%q]', key)
		else
			return key
		end
	else
		return string.format('[%s]', key)
	end
end

function list.inputform_liststring(context, it, lp, tab)
	local hiddent
	if it[0] then
		local headattrib = context.attributes[it[0]]
		if headattrib then
			hiddent = headattrib.hidden
		end
	end
	local itemstr = {}
	local deep = false
	local kitemstr = {}
	for k, item in ipairs(it) do
		local vstr = context:inputform_itemstring(item, lp .. tab, tab)
		itemstr[k] = vstr
		if type(item) == 'table' then
			deep = true
		end
	end
	for k, item in pairs(it) do
		if k ~= 0 and not itemstr[k] then
			if not hiddent or not hiddent[k] then
				local vstr = context:inputform_itemstring(item, lp .. tab, tab)
				table.append(kitemstr, {k, vstr})
				deep = true
			end
		end
	end
	context:sort(kitemstr)
	for i, kitem in ipairs(kitemstr) do
		local kstr = inputform_keystring(kitem[1])
		local str = string.format('%s = %s', kstr, kitem[2])
		table.append(itemstr, str)
	end
	if deep then
		if it[0] then
			return string.format('{[0]=%q,\n%s%s}',
				it[0], lp..tab, table.concat(itemstr, ',\n'..lp..tab))
		else
			return string.format('{\n%s%s}',
				lp..tab, table.concat(itemstr, ',\n'..lp..tab))
		end
	elseif #itemstr == 0 then
		if it[0] then
			return string.format('{[0]=%q}',
				it[0])
		else
			return '{}'
		end
	else
		if it[0] then
			return string.format('{[0]=%q, %s}',
				it[0], table.concat(itemstr, ', '))
		else
			return string.format('{%s}',
				table.concat(itemstr, ', '))
		end
	end
end

function list.inputform_itemstring(context, it, lp, tab)
	local itype = type(it)
	if itype == 'table' then
		return context:inputform_liststring(it, lp, tab)
	elseif itype == 'string' then
		return string.format('%q', it)
	else
		return tostring(it)
	end
end

function list.inputform(context, it)
	return context:inputform_itemstring(it, '', '\t')
end

function list.outputform_liststring(context, it, lp, tab)
	local hiddent
	if it[0] then
		local headattrib = context.attributes[it[0]]
		if headattrib then
			hiddent = headattrib.hidden
		end
	end
	local itemstr = {}
	local deep = false
	local kitemstr = {}
	for k, item in ipairs(it) do
		local vstr = context:outputform_itemstring(item, lp .. tab, tab)
		itemstr[k] = vstr
		if type(item) == 'table' then
			deep = true
		end
	end
	for k, item in pairs(it) do
		if k ~= 0 and not itemstr[k] then
			if not hiddent or not hiddent[k] then
				local vstr = context:outputform_itemstring(item, lp .. tab, tab)
				table.append(kitemstr, {k, vstr})
				deep = true
			end
		end
	end
	context:sort(kitemstr)
	for i, kitem in ipairs(kitemstr) do
		local str = string.format('%s -> %s', kitem[1], kitem[2])
		table.append(itemstr, str)
	end
	if deep then
		if it[0] then
			return string.format('%s[\n%s%s]',
				it[0], lp..tab, table.concat(itemstr, ',\n'..lp..tab))
		else
			return string.format('{\n%s%s}',
				lp..tab, table.concat(itemstr, ',\n'..lp..tab))
		end
	else
		if it[0] then
			return string.format('%s[%s]',
				it[0], table.concat(itemstr, ', '))
		else
			return string.format('{%s}',
				table.concat(itemstr, ', '))
		end
	end
end

function list.outputform_itemstring(context, it, lp, tab)
	local itype = type(it)
	if itype == 'table' then
		return context:outputform_liststring(it, lp, tab)
	elseif itype == 'string' then
		return string.format('%q', it)
	else
		return tostring(it)
	end
end

function list.outputform(context, it)
	return context:outputform_itemstring(it, '', '\t')
end

local typeorder =
	{
		['nil'] = 0,
		['number'] = 1,
		['boolean'] = 2,
		['string'] = 3,
		['table'] = 4,
		['function'] = 5}

function list.compare(a,b)
	local itype = type(a)
	local btype = type(b)
	if itype ~= btype then
		return typeorder[itype] - typeorder[btype]
	elseif itype == 'number' or itype == 'string' then
		return a < b and -1 or a > b and 1 or 0
	elseif itype == 'boolean' then
		return (not a and b) and -1 or (a and not b) and 1 or 0
	elseif itype == 'table' then
		local hd = list.compare(a[0], b[0])
		if hd ~= 0 then
			return hd
		end
		for i = 1, #a do
			local id = list.compare(a[i], b[i])
			if id ~= 0 then
				return id
			end
		end
		return list.compare(a[#a+1], b[#a+1])
	else
		return 0
	end
end

function list.le(a,b)
	return list.compare(a,b) < 0
end

function list.eq(a,b)
	return list.compare(a,b) == 0
end

function list.sort(context, it)
	table.sort(it, list.le)
end

function list.normalize(context, it)
	local headattrib = it[0] and context.attributes[it[0]]
	local flat = headattrib and headattrib.flat
	local orderless = headattrib and headattrib.orderless
	local ret
	if flat then
		ret = {[0]=it[0]}
		for i, item in ipairs(it) do
			if type(item) == 'table' then
				local nitem = context:normalize(item)
				if nitem[0] == it[0] then
					for j, subitem in ipairs(nitem) do
						table.append(ret, subitem)
					end
				else
					table.append(ret, nitem)
				end
			else
				table.append(ret, item)
			end
		end
	else
		ret = context:copy(it)
	end
	if orderless then
		context:sort(ret)
	end
	return ret
end

function list.copy(context, it)
	local ret = {[0]=it[0]}
	for i, v in ipairs(it) do
		ret[i] = v
	end
	return ret
end

function list.copy_all(context, it, target)
	local ret = target or {}
	for i, v in pairs(it) do
		ret[i] = v
	end
	return ret
end

function list.match_structured(context, it, pattern, ct)
	if #it ~= #pattern or it[0] ~= pattern[0] then
		return nil
	end
	local ct = ct or {}
	for i, pitem in ipairs(pattern) do
		if not context:match(it[i], pitem, ct) then
			return nil
		end
	end
	return ct
end

function list.match_ordered(context, it, pattern, ct)
	local itlen = #it
	local patlen = #pattern
	if itlen == patlen then
		return context:match_structured(it, pattern, ct)
	elseif it[0] ~= pattern[0] or itlen < patlen then
		return nil
	end
	for offset = 0, itlen - patlen do
		local subct = {}
		for j = 1, patlen do
			if not context:match(it[j+offset], pattern[j], subct) then
				goto else_for
			end
		end
		if ct then
			context:copy_all(subct, ct)
		else
			ct = subct
		end
		do return ct, offset+1, offset+patlen end
	::else_for::
	end
end

function list.match_orderless(context, it, pattern, ct)
	local itlen = #it
	local patlen = #pattern
	if itlen == patlen then
		return context:match_structured(it, pattern, ct)
	elseif it[0] ~= pattern[0] or itlen < patlen then
		return nil
	end
	local pi = 1
	local subct = {}
	local indices = {}
	for i = 1, itlen do
		if pi > patlen then
			break
		end
		if context:match(it[i], pattern[pi], subct) then
			indices[i] = true
			pi = pi + 1
		end
	end
	if pi <= patlen then
		return nil
	end
	if ct then
		context:copy_all(subct, ct)
	else
		ct = subct
	end
	return ct, indices
end

function list.match(context, it, pattern, ct)
	local itype = type(it)
	local ptype = type(pattern)
	if ptype == 'function' then
		return pattern(context, it, ct)
	elseif itype ~= ptype then
		return nil
	elseif itype ~= 'table' then
		return it == pattern and ct or nil
	else
		if it[0] then
			local headattrib = context.attributes[it[0]]
			if headattrib and headattrib.flat then
				if headattrib.orderless then
					return context:match_orderless(it, pattern, ct)
				else
					return context:match_ordered(it, pattern, ct)
				end
			else
				return context:match_structured(it, pattern, ct)
			end
		else
			return context:match_structured(it, pattern, ct)
		end
	end
	return ct
end

function list.isany(context, it, ct)
	return it
end

function list.isnumber(context, it, ct)
	return type(it) == 'number'
end

function list.isstring(context, it, ct)
	return type(it) == 'string'
end

function list.isboolean(context, it, ct)
	return type(it) == 'boolean'
end

function list.headtest(head)
	return function(context, it, ct)
		return type(it) == 'table' and it[0] == head
	end
end

function list.variant(list)
	return function(context, it, ct)
		for i, pattern in ipairs(list) do
			if context:match(it, pattern, ct) then
				return true
			end
		end
	end
end

function list.capture(key, pattern)
	return function(context, it, ct)
		ct = ct or {}
		if not pattern or context:match(it, pattern, ct) then
			ct[key] = it
			return ct
		end
	end
end

function list.yield(key)
	return function(context, ct)
		local ret = ct[key]
		if type(ret) == 'table' then
			return context:copy(ret)
		else
			return ret
		end
	end
end

local function gmatch_iter(state)
	while true do
		local entry = state.stack[#state.stack]
		if not entry then
			return
		end
		entry[2] = entry[2] + 1
		local elem = entry[1][entry[2]]
		if elem == nil then
			table.pop(state.stack)
			local ct = state.context:match_structured(entry[1], state.pattern)
			if ct then
				return entry[1], ct, state.stack
			end
		else
			if type(elem) == 'table' then
				table.append(state.stack, {elem, 0})
			end
		end
	end
end

function list.gmatch(context, it, pattern)
	local state =
		{
		context = context,
		stack = {{it, 0}},
		pattern = pattern,
		}
	return gmatch_iter, state
end

function list.apply_template(context, template, ct)
	local ttype = type(template)
	if ttype == 'function' then
		return template(context, ct)
	elseif ttype ~= 'table' then
		return template
	else
		local ret = {}
		ret[0] = context:apply_template(template[0], ct)
		for i, titem in ipairs(template) do
			ret[i] = context:apply_template(titem, ct)
		end
		return ret
	end
end

local function rule_next(state, i)
	while state[1] do
		i = i + 1
		local rule = state[1][i]
		if rule then
			return i, rule
		else
			state[1] = state[1].next
			i = 0
		end
	end
	return nil
end

local function rule_iterator(rules)
	return rule_next, {rules}, 0
end

local function replace_structured(context, it, rules)
	for i, rule in rule_iterator(rules) do
		local ct = context:match(it, rule[1])
		if ct and rule[3] then
			ct = rule[3](context, ct)
		end
		if ct then
			return context:apply_template(rule[2], ct)
		end
	end
	return nil
end

local function replace_ordered(context, it, rules)
	for i, rule in rule_iterator(rules) do
		local pattern = rule[1]
		local ptype = type(pattern)
		if ptype == 'table' then
			local ct, first, last = context:match_ordered(it, pattern)
			if ct and rule[3] then
				ct = rule[3](context, ct)
			end
			if ct then
				if first then
					local ret = {[0]=it[0]}
					for j = 1, first-1 do
						table.append(ret, it[j])
					end
					local target = context:apply_template(rule[2], ct)
					if target[0] == it[0] then
						for j, titem in ipairs(target) do
							table.append(ret, titem)
						end
					else
						table.append(ret, target)
					end
					for j = last+1, #it do
						table.append(ret, it[j])
					end
					context:transfer_attrs(ret, it)
					do return ret end
				else
					return context:apply_template(rule[2], ct)
				end
			end
		else
			local ct = context:match(it, pattern)
			if ct and rule[3] then
				ct = rule[3](context, ct)
			end
			if ct then
				return context:apply_template(rule[2], ct)
			end
		end
	end
	return nil
end

local function replace_orderless(context, it, rules)
	for i, rule in rule_iterator(rules) do
		local pattern = rule[1]
		local ptype = type(pattern)
		if ptype == 'table' then
			local ct, indices = context:match_orderless(it, pattern)
			if ct and rule[3] then
				ct = rule[3](context, ct)
			end
			if ct then
				if indices then
					local ret = {[0]=it[0]}
					for j = 1, #it do
						if not indices[j] then
							table.append(ret, it[j])
						end
					end
					local target = context:apply_template(rule[2], ct)
					if target[0] == it[0] then
						for j, titem in ipairs(target) do
							table.append(ret, titem)
						end
					else
						table.append(ret, target)
					end
					context:sort(ret)
					do return ret end
				else
					return context:apply_template(rule[2], ct)
				end
			end
		else
			local ct = context:match(it, pattern)
			if ct and rule[3] then
				ct = rule[3](context, ct)
			end
			if ct then
				return context:apply_template(rule[2], ct)
			end
		end
	end
	return nil
end

function list.replace(context, it, rules)
	if type(it) == 'table' and it[0] then
		local headattrib = context.attributes[it[0]]
		if headattrib and headattrib.flat then
			if headattrib.orderless then
				return replace_orderless(context, it, rules)
			else
				return replace_ordered(context, it, rules)
			end
		else
			return replace_structured(context, it, rules)
		end
	end
	return replace_structured(context, it, rules)
end

function list.replacerepeated(context, it, rules, iter)
	local first = it
	local prev = context:replace(it, rules)
	if not prev then
		return context:copy(it), iter
	end
	for i = iter+1 or 1, context.maxiterations do
		local next = context:replace(prev, rules)
		if not next then
			return prev, i
		end
		prev = next
	end
	log(context:outputform(first))
	log(context:outputform(rules))
	error('maximum number of iterations exceeded')
end

function list.replaceall(context, it, rules, iter)
	if type(it) ~= 'table' then
		return it, iter
	end
	iter = iter or 0
	while true do
		it, iter = context:replacerepeated(it, rules, iter)
		local niter = iter
		if type(it) == 'table' then
			for i, item in ipairs(it) do
				it[i], niter = context:replaceall(item, rules, niter)
			end
			it = context:normalize(it)
		else
			break
		end
		if niter == iter then
			break
		end
		iter = niter
	end
	return it, iter
end

list.attributes = {}
list.maxiterations = 0x10000
