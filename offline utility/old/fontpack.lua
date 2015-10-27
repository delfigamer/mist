local bit = require('bit')

--[[
	struct stream_header {
		beu16 height;
		beu16 descent;
		beu16 maxcharwidth;
		beu16 datasize256;
		beu32 rangebegin;
		beu32 rangeend;
	};

	struct char_header {
		beu16 width;
		beu16 height;
		bes16 xoffset;
		bes16 yoffset;
		bes16 advance;
		beu16 stride;
		beu32 data_offset;
	};
]]

local argf = table.concat(arg)
local args = {}
do
	local argf, lerr = load(argf, '', 't', args)
	if argf then
		local suc, err = pcall(argf)
		if not suc then
			print(err)
		end
	else
		print(lerr)
	end
end

local input = args.input and io.open(args.input, 'r') or io.input()

local function fetchentry()
	local buffer = ''
	local bracecount = 0
	while true do
		local char = input:read(1)
		if not char or #char == 0 then
			error('unexpected end of input stream')
		end
		if char == '{' then
			bracecount = bracecount + 1
		elseif char == '}' then
			bracecount = bracecount - 1
		elseif char == ',' then
			if bracecount == 0 then
				break
			end
		end
		buffer = buffer .. char
	end
	local argf, lerr = load('return ' .. buffer, '', 't', {})
	if argf then
		local suc, result = pcall(argf)
		if suc then
			return result
		else
			error(result)
		end
	else
		print('faulty chunk:', string.format('%q', 'return ' .. buffer))
		error(lerr)
	end
end

local function beu32(i)
	return string.char(
		bit.rshift(i, 24),
		bit.band(bit.rshift(i, 16), 0xff),
		bit.band(bit.rshift(i, 8), 0xff),
		bit.band(i, 0xff))
end

local function beu16(i)
	return string.char(
		bit.rshift(i, 8),
		bit.band(i, 0xff))
end

local function bes16(i)
	if i < 0 then
		i = i + 0x10000
	end
	return beu16(i)
end

local fontinfo = fetchentry()
assert(fontinfo.height + fontinfo.descent + fontinfo.range[1] + fontinfo.range[2])	-- dem hacks
local charmeta = {}
local chardata = {}
local stream_offset = 16 + (fontinfo.range[2] - fontinfo.range[1] + 1) * 16
for charindex = fontinfo.range[1], fontinfo.range[2] do
	local char = fetchentry()
	assert(char.index == charindex)
	local meta = {
		width = char.width + 0,
		height = char.height + 0,
		xoffset = char.xoffset + 0,
		yoffset = char.yoffset + 0,
		advance = char.advance + 0,
		stride = char.stride + 0,
		data_offset = stream_offset,
	}
	local data = {}
	for i = 1, meta.height do
		data[i] = string.char(unpack(char.data[i]))
	end
	data = table.concat(data)
	stream_offset = stream_offset + #data
	local tindex = charindex - fontinfo.range[1] + 1
	charmeta[tindex] =
		beu16(meta.width) .. beu16(meta.height) ..
		bes16(meta.xoffset) .. bes16(meta.yoffset) ..
		bes16(meta.advance) .. beu16(meta.stride) ..
		beu32(meta.data_offset)
	chardata[tindex] = data
end
assert(not fetchentry())

local output = args.output and io.open(args.output, 'wb') or io.output()
output:write(
	beu16(fontinfo.height) .. beu16(fontinfo.descent) ..
	beu16(fontinfo.maxcharwidth) .. '\0\0' ..
	beu32(fontinfo.range[1]) .. beu32(fontinfo.range[2]+1))
for i, d in ipairs(charmeta) do
	output:write(d)
end
for i, d in ipairs(chardata) do
	output:write(d)
end
