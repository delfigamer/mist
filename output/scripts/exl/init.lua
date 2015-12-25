local modname = ...
local exl = package.modtable(modname)

local access_mode = {
	['B'] = 'inout',
	['L'] = 'out',
	['N'] = 'none',
	['R'] = 'in',
}

local function deserializeident(str, start)
	local parts = {}
	while true do
		local length = string.match(str, '^%d%d*', start)
		if not length then
			break
		end
		start = start + #length
		length = tonumber(length)
		if str:sub(start, start) == '_' then
			start = start + 1
		end
		local part = str:sub(start, start + length - 1)
		if #part ~= length then
			error('invalid serial')
		end
		table.append(parts, part)
		start = start + length
		if str:sub(start, start) == '_' then
			start = start + 1
		else
			break
		end
	end
	if #parts == 0 then
		error('invalid serial')
	end
	return table.concat(parts, '.'), start
end

local function deserializeargs(str, start)
	local length = string.match(str, '^%d%d*', start)
	if not length then
		error('invalid serial')
	end
	start = start + #length
	local args = {}
	for i = 1, tonumber(length) do
		local arg
		arg, start = exl.deserializetype(str, start)
		table.append(args, arg)
	end
	return table.concat(args, ', '), start
end

function exl.deserializetype(str, start)
	start = start or 1
	local am = string.sub(str, start, start)
	am = access_mode[am]
	if am == 'none' then
		return am, start + 1
	elseif am then
		start = start + 1
	end
	if am then
		am = am .. ' '
	else
		am = ''
	end
	local first = string.sub(str, start, start)
	if first == 'c' then
		local classname
		classname, start = deserializeident(str, start + 1)
		return am .. 'typeof ' .. classname, start
	elseif first == 'f' then
		local args, rettype
		args, start = deserializeargs(str, start + 1)
		rettype, start = exl.deserializetype(str, start)
		if rettype == 'none' then
			return am .. 'type function(' .. args .. ')', start
		else
			return am .. 'type function(' .. args .. '): ' .. rettype, start
		end
	elseif first == 'n' then
		return am .. 'number', start + 1
	elseif first == 's' then
		return am .. 'string', start + 1
	elseif first == 'v' then
		return am .. 'nil', start + 1
	else
		local classname
		classname, start = deserializeident(str, start)
		return am .. classname, start
	end
end
