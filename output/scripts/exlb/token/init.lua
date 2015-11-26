local modname = ...
local token = package.modtable(modname)

token.codemap = {
	{'v_number', 'rd'},
	{'v_string', 'rs'},
	{'v_function', 'rvl'},
	{'v_function_end', 'base'},

	{'a_add', 'rrr'},
	{'a_sub', 'rrr'},
	{'a_mul', 'rrr'},
	{'a_div', 'rrr'},
	{'a_concat', 'rrr'},

	{'a_createl', 'lr'},
	{'a_setl', 'lr'},
	{'a_getl', 'rl'},

	{'a_return', 'vr'},

	{'d_filepos', 'ii'},
	{'d_filename', 's'},
}
for i, ct in ipairs(token.codemap) do
	ct.code = i
	ct.name = ct[1]
	ct.format = ct[2]
	token.codemap[ct.name] = ct
end

function token:create(cname, ...)
	local ct = self.codemap[cname]
	if not ct then
		error('unknown code: ' .. cname)
	end
	local class = require('exlb.token.' .. ct.format)
	return class:create(ct, ...)
end
