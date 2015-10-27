local modname = ...
local object = require('base.object')
local stream = object:module(modname)

function stream:init_file(path)
	self.lines = {}
	self.linelen = {}
	local i = 0
	for line in io.lines(path) do
		i = i + 1
		self.lines[i] = line
		self.linelen[i] = #line
	end
end

function stream:init_text(text)
	self.lines = {}
	self.linelen = {}
	local i = 0
	local spos = 1
	repeat
		i = i + 1
		local lspos, lepos = text:find('\r\n', spos, true)
		if not lspos then
			lspos, lepos = text:find('[\r\n]', spos)
			if not lspos then
				break
			end
		end
		self.lines[i] = text:sub(spos, lspos - 1)
		self.linelen[i] = lspos - spos
		spos = lepos + 1
	until false
	if spos <= #text then
		self.lines[i] = text:sub(spos)
		self.linelen[i] = #text - spos + 1
	end
end

function stream:init(it)
	if type(it) == 'string' then
		self:init_text(it)
	elseif it.text then
		self:init_text(it.text)
	elseif it.path then
		self:init_file(it.path)
	else
		self.lines = {}
		self.linelen = {}
	end
end

function stream.instmeta:__tostring()
	return string.format('%q', table.concat(self.lines, '\n'))
end
