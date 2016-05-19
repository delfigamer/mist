local modname = ...
local object = require('base.object')
local inputhandler = object:module(modname)
local conf = require('config.index')
local input = require('system.input')

function inputhandler:init(page)
	self.page = page
end

function inputhandler:register()
	input.handlers[2] = self
end

function inputhandler:unregister()
	input.handlers[2] = nil
end

function inputhandler:handle(message, ...)
	local hf = self.page['on'..message]
	if hf then
		hf(self.page, message, ...)
	end
end
