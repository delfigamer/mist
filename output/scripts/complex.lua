local modname = ...
local ffipure = require('base.ffipure')
local complex = ffipure:module(modname)

complex.fields = [[
	float re;
	float im;
]]
complex.cname = 'complex_t'

function complex:create(re, im)
	return self:new(re or 0, im or 0)
end

function complex:createpolar(r, phi)
	return self:new(r * math.cos(phi), r * math.sin(phi))
end

function complex.instmeta.__unm(a)
	return complex:create(- a.re, - a.im)
end

function complex.instmeta.__add(a, b)
	if type(a) == 'number' then
		return complex:create(a + b.re, b.im)
	elseif type(b) == 'number' then
		return complex:create(a.re + b, a.im)
	else
		return complex:create(a.re + b.re, a.im + b.im)
	end
end

function complex.instmeta.__sub(a, b)
	if type(a) == 'number' then
		return complex:create(a - b.re, - b.im)
	elseif type(b) == 'number' then
		return complex:create(a.re - b, a.im)
	else
		return complex:create(a.re - b.re, a.im - b.im)
	end
end

function complex.instmeta.__mul(a, b)
	if type(a) == 'number' then
		return complex:create(a * b.re, a * b.im)
	elseif type(b) == 'number' then
		return complex:create(a.re * b, a.im * b)
	else
		return complex:create(
			a.re * b.re - a.im * b.im,
			a.re * b.im + a.im * b.re)
	end
end

function complex.instmeta.__tostring(a)
	if a.im >= 0 then
		return string.format('%s+%si', a.re, a.im)
	else
		return string.format('%s-%si', a.re, - a.im)
	end
end

function complex:abs()
	return math.sqrt(self.re * self.re + self.im * self.im)
end

function complex:arg()
	return math.atan2(self.im, self.re)
end

complex:buildmetatype()
