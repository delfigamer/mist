local modname = ...
local defer = require('base.defer')
local host = require('host')
local ml = require('host.methodlist').utils
local taskqueue = require('host.taskqueue')

local tqptr = ml.taskqueue_default()
if tqptr == nil then
	host.checkerror()
end
local queue = package.modtable(modname, taskqueue:wrap(tqptr))

defer.push(queue.iterate, queue)
