do
	local gt = setmetatable({}, {
		__index = _G,
		__newindex = function(t,i,v)
			error('attempt to set global ' .. tostring(i))
		end,
	})
	_G = gt
	setfenv(1, gt)
end

local files = {...}
local structname = _G.structname or error('structname expected')
local fileprefix = _G.fileprefix or error('fileprefix expected')

function table.append(arr, item)
	local pos = #arr + 1
	arr[pos] = item
	return pos
end

local function emit_hpp()
	local f = assert(io.open(fileprefix .. '.hpp', 'w'))
	f:write('#pragma once\n\nstruct ')
	f:write(structname)
	f:write('_t\n{\n\z
		\tchar const* name;\n\z
		\tchar const* data;\n\z
		\tint length;\n\z
		};\n\z
		extern ')
	f:write(structname)
	f:write('_t const ')
	f:write(structname)
	f:write('[];')
	f:close()
end

local function emit_cpp()
	local f = assert(io.open(fileprefix .. '.cpp', 'w'))
	f:write('#include "')
	f:write(string.match(fileprefix, '[^\\/]*$'))
	f:write('.hpp"\n\n')
	f:write(structname)
	f:write('_t const ')
	f:write(structname)
	f:write('[] = {\n')
	for i, file in ipairs(files) do
		local name = string.match(file, '([%a_][%w_]*)%..-$') or
			string.match(file, '([%a_][%w_]*)$')
		local input = assert(io.open(file, 'rb'))
		f:write('\t{\n\t\t"')
		f:write(name)
		f:write('",\n')
		while true do
			local str = input:read(8)
			if not str then
				break
			end
			f:write('\t\t\t"')
			for i = 1, #str do
				f:write(string.format('\\x%.2x', string.byte(str, i, i)))
			end
			f:write('"\n')
		end
		f:write('\t\t,\n\t\t')
		f:write(tostring(input:seek()))
		f:write(',\n\t},\n')
	end
	f:write('\t{ 0, 0, 0 },\n};\n')
	f:close()
end

local function main()
	emit_hpp()
	emit_cpp()
end

local suc, err = xpcall(main, debug.traceback)
if not suc then
	collectgarbage()
	print(err)
	os.execute('rm '..structname..'.cpp '..structname..'.hpp')
end
