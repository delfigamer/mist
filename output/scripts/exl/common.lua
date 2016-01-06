local modname = ...
local common = package.modtable(modname)
local default
local lexer
local nodeerror

function common.createnode(pr)
	local class = package.relrequire(modname, 1, 'node.' .. pr.name)
	return class:create(pr)
end

function common.identstring(ident)
	if lexer.keyword[ident] or not ident:match('^[%a_][%w_]*$') then
		return '@' .. tostring(ident)
	else
		return tostring(ident)
	end
end

local escape_start = table.makeset{
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '_',
}

function common.identserial(ident)
	local parts = string.split(ident, '.', true)
	for i, part in ipairs(parts) do
		if escape_start[string.sub(part, 1, 1)] then
			parts[i] = string.format('%i_%s', #part, part)
		else
			parts[i] = string.format('%i%s', #part, part)
		end
	end
	return table.concat(parts, '_')
end

function common.nodeerror(message, node)
	nodeerror:throw(message, node)
end

default = package.relrequire(modname, 1, 'node.default')
lexer = package.relrequire(modname, 1, 'parser.lexer')
nodeerror = package.relrequire(modname, 1, 'nodeerror')
