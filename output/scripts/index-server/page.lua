local modname = ...
local object = require('base.object')
local page = object:module(modname)
local clearshape = require('host.clearshape')
local conenv = require('game.conenv')
local conf = require('config.index-server')
local ffi = require('ffi')
local gifield = require('gi.gifield')
local info = require('host.info')
local input = require('system.input')
local mainconf = require('main')
local resourceset = require('rs.resourceset')
local shapegroup = require('host.shapegroup')
local utility = require('base.utility')
local window = require('host.window')
local worldclass = require('game.world')

function page:init()
	self.shapegroup = shapegroup:create()
	self.clearshape = clearshape:create()
	self.clearshape:setcolor(0, 0, 0, 1)
	self.shapegroup:insert(self.clearshape, -10)
	self.gifield = gifield:create(conf.gilayout)
	self.shapegroup:insert(self.gifield.shapegroup)
	self.input = {
		line = {},
		position = 1,
	}
end

function page:register()
	input.handlers[2] = self.gifield
	input.handlers[3] = self
	window:setshape(self.shapegroup)
	self.world = worldclass:create()
	conenv.printtext = utility.method(self, self.printtext)
	conenv.exit = utility.method(self, self.conexit)
	conenv.world = self.world
	self:setinputline('> _')
	self.tickss = self.world:requiresubsystem('game.ticksubsystem')
	local displayss = self.world:requiresubsystem('game.displaysubsystem')
	self.shapegroup:insert(displayss.shapegroup)
	if mainconf.conline then
		self:conline(mainconf.conline)
	end
end

function page:unregister()
	self.world:release()
	input.handlers[3] = nil
	input.handlers[2] = nil
	self.inputhandler:unregister()
end

function page:handle(message, ...)
	local messagehandler = self['handle_' .. message]
	if messagehandler then
		messagehandler(self, message, ...)
	end
end

function page:printline(line)
	local con = self.gifield.controls['con']
	if con then
		for i = #con.labels, 2, -1 do
			con:setline(i, con:getline(i - 1))
		end
		con:setline(1, line:gsub('\t', '   '))
	end
end

function page:printtext(text)
	text = tostring(text)
	local spos = 1
	local epos = text:find('\n', 1, true)
	while epos do
		self:printline(text:sub(spos, epos))
		spos = epos + 1
		epos = text:find('\n', spos, true)
	end
	self:printline(text:sub(spos))
end

function page:conexit()
	window:finish()
end

function page:conline(line)
	self:printline('> ' .. line)
	local func, err = loadstring('print(' .. line .. ')', 'console input', 't', conenv)
	if func then
		local suc, err = pcall(func)
		if not suc then
			self:printtext(err)
		end
	else
		local func, err = loadstring(line, 'console input', 't', conenv)
		if func then
			local suc, err = pcall(func)
			if not suc then
				self:printtext(err)
			end
		else
			self:printtext(err)
		end
	end
end

function page:setinputline(line)
	local inputline = self.gifield.controls['input']
	if inputline then
		inputline:settext(line)
	end
end

function page:handle_tick(message, dt)
	self.tickss:advance(dt)
	self.world:releasekilledactors()
end

function page:handle_char(message, char)
	if char == '\8' then
		if self.input.position > 1 then
			self.input.position = self.input.position - 1
			self.input.line[self.input.position] = nil
		end
	elseif char == '\13' then
		self:conline(table.concat(self.input.line))
		self.input.line = {}
		self.input.position = 1
	else
		if char == '\9' then
			char = '   '
		end
		self.input.line[self.input.position] = char
		self.input.position = self.input.position + 1
	end
	self:setinputline('> ' .. table.concat(self.input.line) .. '_')
end
