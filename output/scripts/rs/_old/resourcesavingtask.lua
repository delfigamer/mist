local modname = ...
local threadtask = require('tasking.threadtask')
local resourcesavingtask = threadtask:module(modname)
local metadataformat = require('rs.format.metadata')
local types = require('host.types')

function resourcesavingtask:init(taskpriority, resource)
	threadtask.init(self, taskpriority)
	self.resource = resource
end

function resourcesavingtask:threadaction()
	local resource = self.resource
	resource:beforesaving(self)
	local itemio = resource.item:getitemio(true)
	local tag = types.itemtag()
	local metaio = resource.item:getmetaio(true)
	self:yield{
		subtask = metadataformat:startsave(metaio, tag, resource, self.priorityvalue + 1)}
	resource.item:setmetalength(tag.lo)
	self:yield{
		subtask = resource.format:startsave(itemio, tag, resource, self.priorityvalue + 1)}
	resource.item:settag(tag)
	resource.saving = nil
end
