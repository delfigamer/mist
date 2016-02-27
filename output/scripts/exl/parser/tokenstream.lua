local modname = ...
local object = require('base.object')
local tokenstream = object:module(modname)
local lexer = require(modname, 1, 'lexer')
local bufstream = require(modname, 1, 'bufstream')
local exlerror = require(modname, 2, 'exlerror')

function tokenstream:init(istream, filename)
	self.istream = istream
	self.bufstream = bufstream:create(istream, filename)
	self.buffer = {}
	self.pos = self.bufstream:getpos()
	self.filename = filename
end

function tokenstream:gett()
	local token
	if self.buffer[1] then
		token = table.pop(self.buffer)
	else
		token = lexer.obtaintoken(self.bufstream, self.env)
	end
	self.pos = token:getepos()
	return token
end

function tokenstream:ungett(token)
	table.append(self.buffer, token)
	self.pos = token:getspos()
end

function tokenstream:peekt()
	if not self.buffer[1] then
		self.buffer[1] = lexer.obtaintoken(self.bufstream, self.env)
	end
	return self.buffer[#self.buffer]
end

function tokenstream:getpos()
	return self.pos
end

function tokenstream:error(message, spos, epos)
	exlerror:throw(message, spos, epos, self.filename)
end
