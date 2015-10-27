local modname = ...
local object = require('base.object')
local asset = object:module(modname)
local defer = require('base.defer')
local invoke = require('base.invoke')

function asset:create(name, ...)
	local instance = self:new()
	instance.name = name
	instance:init(...)
	return instance
end

local function asset_load_resume(instance, thread)
	local suc, ret = coroutine.resume(thread)
	if suc then
		if coroutine.status(thread) == 'dead' then
			instance:initload(ret)
			return true
		end
	else
		log(ret)
		instance:loadfailed()
		return true
	end
end

function asset:load(name, stream)
	local instance = self:new()
	instance.name = name
	instance:beforeload()
	local thread = coroutine.create(self.loaddata)
	local suc, ret = coroutine.resume(thread, self, stream)
	if suc then
		if coroutine.status(thread) == 'dead' then
			instance:initload(ret)
		else
			defer.push(asset_load_resume, instance, thread)
		end
	else
		log(ret)
		instance:loadfailed()
	end
	return instance
end

function asset:beforeload()
end

function asset:loaddata(stream)
	return nil
end

function asset:initload(data)
end

function asset:loadfailed()
end

function asset:isready()
	return true
end

function asset:blockuntilready()
	while not self:isready() do
		defer.run()
	end
end

function asset:save(stream)
	self:blockuntilready()
	return invoke(self.savedata, self, stream)
end

function asset:savedata(stream)
end
