local modname = ...
local common = package.modtable(modname)
local default
local lexer

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

default = package.relrequire(modname, 1, 'node.default')
lexer = package.relrequire(modname, 1, 'parser.lexer')
