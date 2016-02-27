local modname = ...
local object = require('base.object')
local cpos = require(modname, 1, 'cpos')
local bufstream = object:module(modname)
local exlerror = require(modname, 2, 'exlerror')
local ffi = require('ffi')

bufstream.tabsize = 3

function bufstream:init(stream, filename)
	self.stream = stream
	self.filename = filename
	self.buffer = {}
	self.row = 1
	self.col = 1
	self.prevlinelength = -1
end

function bufstream:getc(noconvert)
	if not self.buffer[1] and not self.beof then
		local cbuf = ffi.new('uint8_t[256]')
		local len = self.stream:read(256, cbuf)
		for i = 0, len-1 do
			self.buffer[len-i] = string.char(cbuf[i])
		end
		if len < 256 then
			self.beof = true
		end
	end
	local ch = table.pop(self.buffer) or ''
	if not noconvert and (ch == '\n' or ch == '\r') then
		local nch = self:getc(true)
		if nch ~= '\n' and nch ~= '\r' or nch == ch then
			self:ungetc(nch, true)
		end
		self.prevlinelength = self.col
		self.row = self.row + 1
		self.col = 1
		return '\n'
	else
		if ch == '\t' then
			self.col = self.col + self.tabsize
		elseif ch ~= '' then
			self.col = self.col + 1
		end
		return ch
	end
end

function bufstream:ungetc(ch, noconvert)
	if ch ~= '' then
		if not noconvert and ch == '\n' then
			table.append(self.buffer, '\r')
			table.append(self.buffer, '\n')
			self.row = self.row - 1
			self.col = self.prevlinelength
			self.prevlinelength = -1
		else
			if ch == '\t' then
				self.col = self.col - self.tabsize
			else
				self.col = self.col - 1
			end
			table.append(self.buffer, ch)
		end
	end
end

function bufstream:getpos()
	return cpos:create(self.row, self.col)
end

function bufstream:error(message, spos, epos)
	exlerror:throw(message, spos, epos, self.filename)
end
