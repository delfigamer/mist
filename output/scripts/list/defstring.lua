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
	local lines = {
		'{',
	}
	for i, item in ipairs(list) do
		local vstr = itemstring(item, lp, tab)
		table.append(lines, lp .. tab .. vstr .. ',')
	end
	table.append(lines, lp .. '}')
	return table.concat(lines, '\n')
end

package.modtable(modname, listdefstring)
