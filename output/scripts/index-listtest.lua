local lptype = require('list.ptype')
local ldefstring = require('list.defstring')
local object = require('base.object')

local list = {[0]='member', {[0]='ssa', 32}, {[0]='id', 4, 6}}

print(ldefstring(list))
