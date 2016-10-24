local modname = ...

local listdefstring

local function itemstring(item, lp, tab)
	local itype = type(item)
	if itype == 'table' then
		return listdefstring(item, lp .. tab, tab)
	elseif itype == 'number' or itype == 'boolean' then
		return tostring(item)
	elseif itype == 'string' then
		return string.format('%q', item)
	else
		error('invalid element type')
	end
end

function listdefstring(list, lp, tab)
	lp = lp or ''
	tab = tab or '\t'
	local itemstr = {}
	local deep = false
	for i, item in ipairs(list) do
		local vstr = itemstring(item, lp, tab)
		table.append(itemstr, vstr)
		if type(item) == 'table' then
			deep = true
		end
	end
	if deep then
		return string.format('%s[\n%s%s]',
			list[0] or '', lp..tab, table.concat(itemstr, ',\n'..lp..tab))
	else
		return string.format('%s[%s]',
			list[0] or '', table.concat(itemstr, ','))
	end
end

package.modtable(modname, listdefstring)
