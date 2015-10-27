local modname = ...
local utf8 = package.modtable(modname)
local ffi = require('ffi')
local ml = require('host.methodlist').utils

local utf8encoding = ml.encoding_getencoding(0)
local defaultchar = 0xfffd -- REPLACEMENT CHARACTER �
local chars_t = ffi.typeof('char[?]')

function utf8.encode(buffer, charcode, size)
	local pointsize = ffi.new('size_t[1]')
	local suc = utf8encoding.encode(buffer, charcode, size, pointsize)
	return suc, tonumber(pointsize[0])
end

function utf8.decode(buffer, size)
	local charcode = ffi.new('uint32_t[1]')
	local pointsize = ffi.new('size_t[1]')
	local suc = utf8encoding.decode(buffer, charcode, size, pointsize)
	if suc then
		return suc, tonumber(pointsize[0]), charcode[0]
	else
		return suc, tonumber(pointsize[0])
	end
end

function utf8.char(...)
	local chars = {...}
	if #chars == 0 then
		return ''
	end
	local length = 0
	for i, cc in ipairs(chars) do
		local suc, len = utf8.encode(nil, cc, 0)
		if not suc then
			suc, len = utf8.encode(nil, defaultchar, 0)
		end
		length = length + len
	end
	local buffer = chars_t(length)
	local ptr = buffer
	local buflen = length
	for i, cc in ipairs(chars) do
		local suc, len = utf8.encode(ptr, cc, buflen)
		ptr = ptr + len
		buflen = buflen - len
	end
	return ffi.string(buffer, length)
end

utf8.charpattern = '[%z\x01-\x7f\xc0-\xff][\x80-\xbf]*'

local function codes_next(state, pos)
	if pos >= state.limit then
		return
	end
	local suc, len, cc = utf8.decode(state.buffer + pos, state.limit - pos)
	if not suc then
		if len == 0 then
			len = state.limit - pos
		end
		cc = defaultchar
	end
	return pos + len, cc
end

function utf8.codes(string, first, limit)
	local state = {
		string = string,
		buffer = ffi.cast('char const*', string),
		limit = limit or #string,
	}
	return codes_next, state, first or 0
end
