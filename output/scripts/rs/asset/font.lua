local modname = ...
local asset = require('rs.asset')
local font = asset:module(modname)
local persistent = require('rs.persistent')
local arrayptype = require('rs.arrayptype')
local fontrangeptype = require('rs.fontrangeptype')
local scalars = require('rs.scalars')
local fontdata = persistent:module(
	modname .. ':data',
	{
		{'ranges', arrayptype(fontrangeptype)},
		{'fallbackname', scalars.string, optional = true},
		{'height', scalars.int},
		{'descent', scalars.int},
	}
)

function fontdata:init(it)
	self.ranges = {}
	for i, range in ipairs(it) do
		self.ranges[i] = range
	end
	self.fallbackname = it.fallbackname
	self.height = it.height or self.ranges[1] and self.ranges[1].height
	self.descent = it.descent or self.ranges[1] and self.ranges[1].descent
end

function fontdata:initload()
end

function font:init(ranges, fallbackname)
	self.data = fontdata:create(ranges, fallbackname)
end

function font:loaddata(stream)
	return fontdata.ptype:read(stream)
end

function font:initload(data)
	self.data = data
	if self.data.fallbackname then
		log(self.data.fallbackname)
	end
end

function font:loadfailed()
	self.data = false
end

function font:release()
	if self.data then
		for i, range in ipairs(self.data.ranges) do
			range.db:release()
			range.chars = nil
		end
	end
end

function font:isready()
	return self.data ~= nil
end

function font:savedata(stream)
	if self.data then
		fontdata.ptype:write(self.data, stream)
	else
		error('invalid font object')
	end
end

function font:getchar(charcode)
	self:blockuntilready()
	for i, range in ipairs(self.data.ranges) do
		local index = charcode - range.firstchar
		if index >= 0 and index < range.charcount then
			return range.chars[index]
		end
	end
	if self.fallback then
		return self.fallback:getchar(charcode)
	end
end

function font:setfallbackname(name)
	if self.data then
		self.data.fallbackname = name
	end
end
