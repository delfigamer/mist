local modname = ...
module(modname, package.seeall)

function be32(int)
	return string.char(
		bit.rshift(int, 24),
		bit.band(bit.rshift(int, 16), 0xff),
		bit.band(bit.rshift(int, 8), 0xff),
		bit.band(int, 0xff))
end
	
function checkbitnu(number, bits)
	local mask = bit.lshift(-1, bits)
	if bit.band(number, mask) == 0 then
		return number
	end
end

function checkbitns(number, bits)
	local mask = bit.lshift(-1, bits - 1)
	if bit.band(number, mask) == 0 or bit.band(number, mask) == mask then
		return bit.band(number, bit.rshift(-1, 32 - bits))
	end
end
	
function checkbitnus(number, bits)
	local mask = bit.lshift(-1, bits - 1)
	if bit.band(number, bit.lshift(mask, 1)) == 0 or bit.band(number, mask) == mask then
		return bit.band(number, bit.rshift(-1, 32 - bits))
	end
end

function checkbitn(template, number)
	if template.signed then
		return checkbitns(number, template.bits)
	elseif template.unsigned then
		return checkbitnu(number, template.bits)
	else
		return checkbitnus(number, template.bits)
	end
end

function bitfield(templates, ...)
	local argv = {...}
	local result = 0
	for i, it in ipairs(templates) do
		local value = checkbitn(it, argv[i] or error('number expected at position '..tostring(i)))
		if not value then
			error(string.format(
				'value at position %i (%i) is too large for template %i%s',
				i, argv[i], it.bits, it.signed and 's' or it.unsigned and 'u' or ''))
		end
		result = bit.bor(bit.lshift(result, it.bits), value)
	end
	return result
end

function template(str)
	local ch = str:sub(-1,-1)
	if ch == 's' then
		return {
			signed = true,
			bits = tonumber(str:sub(1,-2))}
	elseif ch == 'u' then
		return {
			unsigned = true,
			bits = tonumber(str:sub(1,-2))}
	else
		return {
			bits = tonumber(str)}
	end
end
