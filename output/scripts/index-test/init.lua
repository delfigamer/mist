local modname = ...
local index = package.modtable(modname)
local ffi = require('ffi')
local window = require('host.window')

local event = ffi.new('event')
while true do
	if window:popevent(event) then
		print(event)
		if event.name == 0 then
			break
		end
	end
end
