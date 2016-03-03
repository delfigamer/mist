local modname = ...
module(modname, package.seeall)
local conf = require('config.input')
local defer = require('base.defer')
local ffi = require('ffi')
local info = require('host.info')
local utility = require('base.utility')

if conf.acceleratorinput == nil then
	conf.acceleratorinput = info.acceleratorinput
end
if conf.pointinput == nil then
	conf.pointinput = info.pointinput
end
if conf.keyboardinput == nil then
	conf.keyboardinput = info.keyboardinput
end

local basehandler = {}

handlers = {
	[1] = basehandler}

function basehandler:handle(message, ...)
	local hf = self['on' .. message]
	if hf then
		hf(self, message, ...)
	else
		log('-> ' .. message .. '(unknown)', ...)
		return true
	end
end

function basehandler:ondestroy(message)
	log('-> destroy')
end

function basehandler:ontick(message, dt)
	defer.run()
end

if conf.acceleratorinput then
	log('accelerator input enabled')

	accelerator = ffi.new('struct {float x; float y; float z;}', 0, 0, 0)

	function basehandler:onacceleration(message, x, y, z)
		accelerator.x, accelerator.y, accelerator.z = x, y, z
	end
end

if conf.pointinput then
	log('point input enabled')

	pointtype = ffi.typeof('struct {int x; int y;}')
	points = {}

	function basehandler:onpointdown(message, point, x, y)
		points[point] = pointtype(x, y)
	end

	function basehandler:onpointup(message, point)
		points[point] = nil
	end

	function basehandler:onpointmove(message, point, x, y)
		local point = points[point]
		point.x, point.y = x, y
	end
end

mb = {
	left = 1,
	right = 2,
	shift = 3,
	control = 4,
	middle = 5}

if conf.keyboardinput then
	log('keyboard input enabled')

	keystates = ffi.new('bool[256]', 0)

	function basehandler:onkeydown(message, key)
		-- if not keystates[key] then
			-- print('keydown', kname[key])
		-- end
		keystates[key] = true
	end

	function basehandler:onkeyup(message, key)
		-- if keystates[key] then
			-- print('keyup', kname[key])
		-- end
		keystates[key] = false
	end

	function basehandler:onchar(message, char)
	end
end

k = {
	bksp = 8,
	tab = 9,
	enter = 13,
	shift = 16,
	ctrl = 17,
	pause = 19,
	esc = 27,
	space = 32,
	pgup = 33,
	pgdown = 34,
	['end'] = 35,
	home = 36,
	left = 37,
	up = 38,
	right = 39,
	down = 40,
	insert = 45,
	delete = 46,
	menu = 93,
	npstar = 106,
	npplus = 107,
	npminus = 109,
	npdot = 110,
	npslash = 111,
	colon = 186,
	equals = 187,
	comma = 188,
	minus = 189,
	period = 190,
	slash = 191,
	tilde = 192,
	lbrace = 219,
	bkslash = 220,
	rbrace = 221,
	quote = 222}
utility.table_insert(
	k, function(i) return string.char(i), i-32 end,
	utility.range(string.byte('a'), string.byte('z')))
utility.table_insert(
	k, function(i) return 'n' .. string.char(i), i end,
	utility.range(string.byte('0'), string.byte('9')))
utility.table_insert(
	k, function(i) return 'np' .. tostring(i), i+96 end,
	utility.range(0, 9))
utility.table_insert(
	k, function(i) return 'f' .. tostring(i), i+111 end,
	utility.range(1, 12))

kname = utility.table(
	function(k, v) return v, k end,
	pairs(k))

function basehandler:onfocusgained(message)
end

function basehandler:onfocuslost(message)
end

function basehandler:onwm(message, index, lparam, wparam)
	log(string.format('-> 0x%.4x\t%.8x\t%.8x', index, lparam, wparam))
end

local function dispatch_a(message, ...)
	for i, handler in ipairs(handlers) do
		if handler:handle(message, ...) then
			break
		end
	end
end

function dispatch(message, ...)
	local suc, err = pcall(dispatch_a, message, ...)
	if not suc then
		log('! Error while handling "'..tostring(message)..'":', err)
	end
end
