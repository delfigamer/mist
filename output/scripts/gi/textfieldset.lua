local modname = ...
local object = require('base.object')
local textfieldset = object:module(modname)
local textfield = require('gi.textfield')

function textfieldset:init()
	self.textfields = {}
end

function textfieldset:gettf(fontname)
	local tf = self.textfields[fontname]
	if not tf then
		tf = textfield:create(fontname)
		self.textfields[fontname] = tf
	end
	return tf
end
