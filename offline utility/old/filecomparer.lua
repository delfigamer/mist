-- 	Difference map layout.
-- 	Entry header (16 bytes):
-- 		uint32_t action;	// 0 - end marker; 1 - copy; 2 - replace; 
-- 		uint32_t sourcelength;
-- 		uint32_t source_crc;	// CRC32, polynomial: 0x82F63B78
-- 		uint32_t targetlength;
-- 	After every entry header, targetlength bytes of new data are located,
-- with ( 15 - ( targetlength + 15 ) % 16 ) bytes of padding at the end, so all
-- entries are 16-byte-aligned.
-- 	Byte order is big-endian.
-- 	This format is intended for embedding into other, more complex formats.
-- As such, it lacks any data dedicated specially to identifying this format
-- among others. It's unadviced to use this map as a standalone file.

require('args')
crc32 = require('crc32')
bit = require('bit')

local blocksize = 0x400	-- 1KB

local sourcepath, targetpath, diffpath = args.files[1], args.files[2], args.files[3]

if not sourcepath or not targetpath or not diffpath then
	print('syntax:')
	print('filecomparer.lua <source> <target> <output>')
	return
end

local function lcs(source, target)
	local cache = {[0] = ''}
	for i = 1, #source do
		cache[i] = ''
	end
	for j = 1, #target do
		local corner = ''
		for i = 1, #source do
			local schar = source:sub(i,i)
			local tchar = target:sub(j,j)
			local result
			if schar == tchar then
				result = corner .. schar
			else
				if #cache[i] > #cache[i-1] then
					result = cache[i]
				else
					result = cache[i-1]
				end
			end
			corner = cache[i]
			cache[i] = result
		end
	end
	return cache[#source]
end

local function compare(source, target, lcs, offset)
 	local ops = {}
	local i = 1
	local j = 1
	local passlength = 0
	for k = 1, #lcs do
		local cchar = lcs:sub(k,k)
		local newi = source:find(cchar, i, true) + 1
		local cutlength = newi - i - 1
		local newj = target:find(cchar, j, true) + 1
		local newdata = target:sub(j, newj - 2)
		if cutlength > 0 or #newdata > 0 then
			if passlength > 0 then
				ops[#ops + 1] = {
					action = 'pass',
					start = i - passlength - 1,
					length = passlength,
					data = lcs:sub(k - passlength, k - 1)}
				passlength = 0
			end
			ops[#ops + 1] = {
				action = 'replace',
				start = i - 1,
				length = cutlength,
				data = newdata}
		end
		i = newi
		j = newj
		passlength = passlength + 1
	end
	if passlength > 0 then
		ops[#ops + 1] = {
			action = 'pass',
			start = i - passlength - 1,
			length = passlength,
			data = lcs:sub(-passlength, -1)}
	end
	if i < #source or j < #target then
		ops[#ops + 1] = {
			action = 'replace',
			start = i - 1,
			length = #source - i + 1,
			data = target:sub(j)}
	end
	return ops
end

local function mergeops(ops, minpassthreshold)
	local opcount = #ops
	local i = 2
	local j = 1
	if ops[1].action == 'pass' and ops[1].length < minpassthreshold then
		ops[1].action = 'replace'
	end
	while i <= opcount do
		local op = ops[i]
		local top = ops[j]
		if op.action == 'pass' and op.length < minpassthreshold then
			op.action = 'replace'
		end
		if op.action == top.action then
			top.length = top.length + op.length
			top.data = top.data .. op.data
		else
			j = j + 1
			ops[j] = op
		end
		i = i + 1
	end
	for i = j + 1, opcount do
		ops[i] = nil
	end
end

local function uint32(i)
	return string.char(
		bit.band(bit.rshift(i, 24), 255),
		bit.band(bit.rshift(i, 16), 255),
		bit.band(bit.rshift(i, 8), 255),
		bit.band(i, 255))
end

local sourcefile = assert(io.open(sourcepath, 'rb'))
local targetfile = assert(io.open(targetpath, 'rb'))
local difffile = assert(io.open(diffpath, 'wb'))
while true do
	local sourcepart = sourcefile:read(blocksize)
	if not sourcepart then
		break
	end
	local targetpart = targetfile:read(blocksize) or ''
	local partlcs = lcs(sourcepart, targetpart)
	local partops = compare(sourcepart, targetpart, partlcs)
	mergeops(partops, 16)
	for i, op in ipairs(partops) do
		if op.action == 'pass' then
			difffile:write('\0\0\0\1')
			difffile:write(uint32(op.length))
			difffile:write(uint32(crc32(op.data)))
			difffile:write('\0\0\0\0')
		elseif op.action == 'replace' then
			difffile:write('\0\0\0\2')
			difffile:write(uint32(op.length))
			local sdata = sourcepart:sub(op.start + 1, op.start + op.length)
			difffile:write(uint32(crc32(sdata)))
			difffile:write(uint32(#op.data))
			difffile:write(op.data)
			difffile:write(string.rep('\0', 15 - math.mod(#op.data + 15, 16)))
		else
			error()
		end
	end
end
while false do
	local targetpart = targetfile:read(blocksize)
	if not targetpart then
		break
	end
	difffile:write('\0\0\0\2')
	difffile:write('\0\0\0\0')
	difffile:write('\0\0\0\0')
	difffile:write(uint32(#targetpart))
	difffile:write(#targetpart)
	difffile:write(string.rep('\0', 15 - math.mod(#targetpart + 15, 16)))
end
difffile:write('\0\0\0\0')
difffile:write('\0\0\0\0')
difffile:write('\0\0\0\0')
difffile:write('\0\0\0\0')
