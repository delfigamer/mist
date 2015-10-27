local modname = ...
local threadtask = require('tasking.threadtask')
local resourceloadingtask = threadtask:module(modname)
local metadataformat = require('rs.format.metadata')
local types = require('host.types')

function resourceloadingtask:init(taskpriority, resource)
	threadtask.init(self, taskpriority)
	self.resource = resource
end

function resourceloadingtask:threadaction()
	local resource = self.resource
	resource.ischanged = false
	local itemio = resource.item:getitemio(false)
	if itemio then
		local tag = resource.item:gettag()
		self:yield{
			subtask = resource.format:startload(itemio, tag, resource, self.priorityvalue + 1)}
	end
	local metaio = resource.item:getmetaio(false)
	if metaio then
		local length = types.itemtag(0, resource.item:getmetalength())
		self:yield{
			subtask = metadataformat:startload(metaio, length, resource, self.priorityvalue + 1)}
	else
		resource.metadata = {}
	end
	resource:afterloading(self)
	resource.loading = nil
end
