local modname = ...
local input = package.modtable(modname)
local defer = require('base.defer')
local ffi = require('ffi')
-- local info = require('host.info')
local window = require('host.window')

-- local event_handlers = {
	-- [0] = 'onclose',
	-- [1] = 'onpointdown',
	-- [2] = 'onpointup',
	-- [3] = 'onpointmove',
	-- [4] = 'onkeydown',
	-- [5] = 'onkeyup',
	-- [6] = 'onchar',
	-- [7] = 'onfocus',
-- }

function basehandler:ondestroy(message)
	log('-> destroy')
end

function basehandler:ontick(message, dt)
	defer.run()
end

if info.acceleratorinput then
	log('accelerator input enabled')

	input.accelerator = ffi.new('struct {float x; float y; float z;}', 0, 0, 0)

	function basehandler:onacceleration(message, x, y, z)
		accelerator.x, accelerator.y, accelerator.z = x, y, z
	end
end

if info.pointinput then
	log('point input enabled')

	input.pointtype = ffi.typeof('struct {int x; int y;}')
	input.points = {}

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

input.mb = {
	left = 1,
	right = 2,
	shift = 3,
	control = 4,
	middle = 5}

if info.keyboardinput then
	log('keyboard input enabled')

	input.keystates = ffi.new('bool[256]', 0)

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

input.k = {
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
for i = string.byte('a'), string.byte('z') do
	input.k[string.char(i)] = i-32
end
for i = string.byte('0'), string.byte('9') do
	input.k['n' .. string.char(i)] = i
end
for i = 0, 9 do
	input.k['np' .. tostring(i)] = i+96
end
for i = 1, 12 do
	input.k['f' .. tostring(i)] = i+111
end

input.kname = {}
for k, v in pairs(input.k) do
	input.kname[v] = k
end

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

function input.dispatch(message, ...)
end

function input.maincycle(target)
	local event = ffi.new('event')
	while true do
		if window:popevent(event) then
			local bh = basehandler[event.name]
			if bh then
				local suc, err = pcall(bh, event)
				if not suc then
					print('! Error while handling ' .. tostring(message) .. ':', err)
				end
			end
			if event.name == 0 then
				break
			end
		end
	end
end
