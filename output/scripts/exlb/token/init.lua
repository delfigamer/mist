local modname = ...
local token = package.modtable(modname)

-- d - double
-- i - int
-- s - VLA of char
-- r - SSA register
-- l - (depth, name) local id
-- v{x} - VLA of {x}

token.codemap = {
	{'v_number', 'rd'},
	{'v_string', 'rs'},
	{'v_function', 'rvl'},
	{'v_function_end', 'base'},
	{'v_table', 'r'},
	{'v_table_end', 'base'},

	{'a_add', 'rrr'},
	{'a_sub', 'rrr'},
	{'a_mul', 'rrr'},
	{'a_div', 'rrr'},
	{'a_concat', 'rrr'},

	{'a_createl', 'lr'},
	{'a_setl', 'lr'},
	{'a_getl', 'lr'},

	{'a_call', 'rvrvr'},
	{'a_return', 'vr'},

	{'d_filepos', 'ii'},
	{'d_filename_start', 's'},
	{'d_filename_end', 'base'},
	{'d_comment', 's'},
}
for i, ct in ipairs(token.codemap) do
	ct.code = i
	ct.name = ct[1]
	ct.format = ct[2]
	ct.class = package.relrequire(modname, 0, ct.format)
	token.codemap[ct.name] = ct
end

function token:create(cname, ...)
	local ct = self.codemap[cname]
	if not ct then
		error('unknown code: ' .. cname)
	end
	return ct.class:create(ct, ...)
end
