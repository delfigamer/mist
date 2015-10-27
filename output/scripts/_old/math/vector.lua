local modname = ...
local object = require('base.object')
local vector = object:module(modname)

vector.default = 0

function vector:init(it)
	if it.length then
		self.length = it.length
	else
		self.length = #it
	end
	for i = 1, self.length do
		self[i] = it[i] or it.default or self.default
	end
end

function vector:scale(factor)
	for i = 1, self.length do
		self[i] = self[i] * factor
	end
end

function vector:dot(other)
	if self.length ~= other.length then
		error('cannot dot vectors with different lengths')
	elseif self.length == 0 then
		return self.default
	end
	local s = self[1] * other[1]
	for i = 2, self.length do
		s = s + self[i] * other[i]
	end
	return s
end

function vector:square()
	return self:dot(self)
end

function vector:len()
	return math.sqrt(self:square())
end

function vector:add(other, factor)
	if self.length ~= other.length then
		error('cannot add vectors with different lengths')
	end
	if factor ~= 0 then
		for i = 1, self.length do
			self[i] = self[i] + other[i] * factor
		end
	elseif not factor then
		for i = 1, self.length do
			self[i] = self[i] + other[i]
		end
	end
end

function vector.instmeta:__tostring()
	local lines = {}
	for i = 1, self.length do
		lines[i] = tostring(self[i])
	end
	return 'vector<' .. table.concat(lines, ',') .. '>'
end

function vector.instmeta:__add(other)
	return vector:create(self):add(other)
end

function vector.instmeta:__sub(other)
	return vector:create(self):add(other, -1)
end

function vector.instmeta:__unm(other)
	return vector:create(self):scale(-1)
end

function vector.instmeta:__mul(other)
	if type(self) ~= vector.instmeta.__type then
		return other * self
	end
	if type(other) == vector.instmeta.__type then
		return self:dot(other)
	else
		return self:scale(other)
	end
end

function vector.instmeta:__len()
	return self:len()
end
