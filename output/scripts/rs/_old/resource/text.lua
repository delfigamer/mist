local modname = ...
local base = require('rs.resource.base')
local text = base:module(modname, 'Text')
local exttable = require('rs.resource.exttable')
local textformat = require('rs.format.text')

exttable['txt'] = text

text.istextresource = true
text.format = textformat
text.searchpath = '?.txt'

function text:createnew()
	self.text = ''
end

function text:gettext()
	self:finishloading()
	return self.text
end

function text:settext(text)
	self:touchwrite()
	self:finishloading()
	self.text = text
end
