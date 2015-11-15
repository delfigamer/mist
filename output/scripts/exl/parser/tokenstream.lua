local modname = ...
local object = require('base.object')
local tokenstream = object:module(modname)
local lexer = require('exl.parser.lexer')
local bufstream = require('exl.parser.bufstream')

function tokenstream:init(istream)
	self.istream = istream
	self.bufstream = bufstream:create(istream)
	self.buffer = {}
	self.pos = self.bufstream:getpos()
end

function tokenstream:gett(context)
	local token
	if self.buffer[1] then
		token = table.pop(self.buffer)
	else
		token = lexer.obtaintoken(self.bufstream, context)
	end
	self.pos = token:getepos()
	return token
end

function tokenstream:ungett(token, context)
	table.append(self.buffer, token)
	self.pos = token:getspos()
end

function tokenstream:peekt(context)
	if not self.buffer[1] then
		self.buffer[1] = lexer.obtaintoken(self.bufstream, context)
	end
	return self.buffer[#self.buffer]
end

function tokenstream:getpos()
	return self.pos
end
