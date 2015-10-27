local modname = ...
local utility = require('base.utility')
local fenv = package.modtable(modname)

function fenv.formatarg(env, arg)
	local func, err = loadstring('return ' .. arg)
	if not func then
		return '<< ' .. err .. ' >>'
	end
	setfenv(func, env)
	local suc, res = xpcall(func, utility.operator.identity)
	if suc then
		return res and tostring(res) or ''
	else
		return '<< ' .. res .. ' >>'
	end
end

function fenv.format(fstr, pt)
	local env
	if pt ~= nil then
		assert(type(pt) == 'table')
		env = setmetatable({}, {
			__index = function(self, index)
				local v = pt[index]
				if v ~= nil then
					return v
				else
					return fenv[index]
				end
			end})
	else
		env = setmetatable({}, {__index = getfenv(2)})
	end
	return string.gsub(fstr, '<<(.-)>>',
		function(str)
			return fenv.formatarg(env, str)
		end)
end

function fenv.wrap(fstr)
	return utility.method_s(fstr, format)
end
