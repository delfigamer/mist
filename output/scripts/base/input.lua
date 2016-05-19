local modname = ...
local input = package.modtable(modname)
local defer = require('base.defer')
local ffi = require('ffi')
-- local info = require('host.info')
local queue = require('base.queue')
local set = require('base.set')
local window = require('host.window')

input.events = table.makeenum{
	close = 0,
	pointdown = 1,
	pointup = 2,
	pointmove = 3,
	keydown = 4,
	keyup = 5,
	char = 6,
	focus = 7,
}

-- if info.acceleratorinput then
	-- log('accelerator input enabled')

	-- input.accelerator = ffi.new('struct {float x; float y; float z;}', 0, 0, 0)

	-- function basehandler:onacceleration(message, x, y, z)
		-- accelerator.x, accelerator.y, accelerator.z = x, y, z
	-- end
-- end

-- if info.pointinput then
	-- log('point input enabled')

	-- input.pointtype = ffi.typeof('struct {int x; int y;}')
	-- input.points = {}

	-- local function base_pointdown(message, point, x, y)
		-- points[point] = pointtype(x, y)
	-- end

	-- local function base_pointup(message, point)
		-- points[point] = nil
	-- end

	-- local function base_pointmove(message, point, x, y)
		-- local point = points[point]
		-- point.x, point.y = x, y
	-- end
-- end

input.mb = {
	left = 1,
	right = 2,
	middle = 3,
}

-- if info.keyboardinput then
	-- log('keyboard input enabled')

	-- input.keystates = ffi.new('bool[256]', 0)

	-- function base_keydown(message, key)
		-- keystates[key] = true
	-- end

	-- function basehandler:onkeyup(message, key)
		-- keystates[key] = false
	-- end

	-- function basehandler:onchar(message, char)
	-- end
-- end

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
	quote = 222,
}
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

-- function basehandler:onfocusgained(message)
-- end

-- function basehandler:onfocuslost(message)
-- end

-- function basehandler:onwm(message, index, lparam, wparam)
	-- log(string.format('-> 0x%.4x\t%.8x\t%.8x', index, lparam, wparam))
-- end

-- local function dispatch_a(message, ...)
	-- for i, handler in ipairs(handlers) do
		-- if handler:handle(message, ...) then
			-- break
		-- end
	-- end
-- end

input.handlers = {}

function input.register(name, handler)
	local hs = input.handlers[name]
	if not hs then
		hs = set:create()
		input.handlers[name] = hs
	end
	hs:insert(handler)
end

function input.unregister(name, handler)
	local hs = input.handlers[name]
	if not hs then
		return
	end
	hs:remove(handler)
end

function input.dispatch(event)
	local hs = input.handlers[event.name]
	if not hs then
		return
	end
	for i, handler in hs:pairs() do
		local suc, ret = pcall(handler, event)
		if not suc then
			print(ret)
		end
	end
end

input.eventqueue = queue:create()

function input.pushevent(event)
	input.eventqueue:push(event)
end

function input.maincycle(target)
	local wevent = ffi.new('struct event')
	while true do
		local event
		if window:popevent(wevent) then
			event = wevent
		else
			event = input.eventqueue:pop()
		end
		if event then
			input.dispatch(event)
			if event.name == 0 then
				break
			end
		end
		defer.run()
	end
end
