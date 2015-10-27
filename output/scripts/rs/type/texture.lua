local modname = ...
local base = require('rs.type.base')
local texture = base:module(modname)
local databuffer = require('host.databuffer')
local pngreader = require('host.pngreader')

local function loadlevel(db)
	local reader = pngreader:create(0)
	reader:feed(db:getlength(), db:getdata())
	if not reader:isfinished() then
		error('png stream terminated unexpectedly')
	end
	return {
		format = 0,
		width = reader:getwidth(),
		height = reader:getheight(),
		data = reader:getdata(),
	}
end

function texture:init()
end

function texture:load(data)
	self.level = {}
	for i = 0, #data:get('level') do
		self.level[i] = loadlevel(data:optdata('level', i) or error('invalid asset'))
	end
end
