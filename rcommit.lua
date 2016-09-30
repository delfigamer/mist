assert(loadfile('base.lua'))()

local list
do
	local log = assert(io.open('build/rpending.lua', 'r'))
	local logcontent = log:read('*a')
	log:close()
	local chunk = assert(load('return {' .. logcontent .. '}'))
	list = assert(chunk())
end

for i, entry in ipairs(list) do
	local content = {}
	local row = 1
	do
		local stream = assert(io.open(entry.path, 'r'))
		while true do
			local line = stream:read('*l')
			if not line then
				break
			end
			if entry[row] then
				assert(line == entry[row][1])
				line = entry[row][2]
			end
			table.append(content, line .. '\n')
			row = row + 1
		end
		stream:close()
	end
	do
		local stream = assert(io.open(entry.path, 'w'))
		stream:write(table.concat(content))
		stream:close()
	end
end
