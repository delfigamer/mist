local modname = ...
module(modname, package.seeall)

function bew(str, pos)
	local a, b, c, d = str:byte(pos, pos + 3)
	return bit.bor(
		bit.lshift(a, 24),
		bit.lshift(b, 16),
		bit.lshift(c, 8),
		d)
end

function bitfield(i, s, e)
	if e then
		return bit.band(bit.rshift(i, 32 - e), bit.rshift(-1, 32 - e + s))
	else
		return bit.band(bit.rshift(i, 31 - s), 1) ~= 0
	end
end

function exts(i, s)
	if bit.rshift(i, s - 1) ~= 0 then
		return bit.bor(i, bit.lshift(-1, s))
	else
		return i
	end
end

function opcd(i)
	return bitfield(i, 0, 6)
end

function xxo(i)
	return bitfield(i, 21, 31)
end
