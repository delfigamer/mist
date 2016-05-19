local modname = ...
module(modname, package.seeall)
local mainconf = require('main')
local window = require('host.window')

local nodes = {
	add = require('index-img.add'),
	convolute = require('index-img.convolute'),
	divide = require('index-img.divide'),
	export = require('index-img.export'),
	functional = require('index-img.functional'),
	generate = require('index-img.generate'),
	import = require('index-img.import'),
	integrate = require('index-img.integrate'),
	modulate = require('index-img.modulate'),
	load = require('index-img.load'),
	locallims = require('index-img.locallims'),
	resample = require('index-img.resample'),
	save = require('index-img.save'),
}

local function getactionlist()
	if mainconf.actionlist then
		return mainconf.actionlist
	elseif mainconf.alfile then
		return assert(loadfile(_PATH .. mainconf.alfile))()
	else
		error('action list required')
	end
end

local function action()
	local pintable = {}
	local actionlist = getactionlist()
	for i, arow in ipairs(actionlist) do
		local current
		if arow[0] then
			current = pintable[arow[0]]
		end
		for j, aconf in ipairs(arow) do
			local name = aconf[1] or error('node name required')
			local nclass = nodes[name] or error('unknown node:' .. name)
			local anode = nclass:create(aconf, current, pintable)
			anode:process()
			current = anode
		end
		if arow.pin then
			pintable[arow.pin] = current
			print('pin ' .. arow.pin .. ' obtained')
		end
	end
end

function register()
	local suc, err = pcall(action)
	if not suc then
		print(err)
	end
	window:finish()
end

function unregister()
end
