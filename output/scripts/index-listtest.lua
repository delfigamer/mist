local lptype = require('list.ptype')
local ldefstring = require('list.defstring')
local lregex = require('list.regex')
local object = require('base.object')

local p_ssa = {
	'ssa',
	lregex.capture('name', lregex.number),
}

local p_id = {
	'id',
	lregex.capture('depth', lregex.number),
	lregex.capture('name', lregex.number),
}

local p_member = {
	'member',
	lregex.invoke('target', lregex.choice(p_ssa, p_id)),
	lregex.invoke('index', p_id),
}

local list = {'member', {'ssa', 32}, {'id', 4, 6}}

local mr = lregex.match(list, p_member)
if mr then
	log(object.defstring(mr))
	log(object.defstring(mr.target))
	log(object.defstring(mr.index))
else
	log('none')
end
