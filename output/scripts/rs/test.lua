local modname = ...
module(modname, package.seeall)
local arrayptype = require('rs.arrayptype')
local databuffer = require('host.databuffer')
local dbptype = require('rs.dbptype')
local iostream = require('rs.iostream')
local memoryio = require('rs.memoryio')
local persistent = require('rs.persistent')
local scalars = require('rs.scalars')
local teststream = require('rs.teststream')

local range = persistent:derive(
	nil,
	modname .. ':psub',
	{
		{'left', scalars.int},
		{'width', scalars.int},
		{'right', scalars.int, transient = true},
	}
)

function range:init(left, width)
	self.left = left
	self.width = width
	self.right = left + width
end

function range:initload()
	self.right = self.left + self.width
end

function range.instmeta:__tostring()
	return string.format('[%i; %i)', self.left or -1, self.right or -1)
end

proplist = {
	{'sub', arrayptype(range.ptype)},
}
local ptest = persistent:derive(nil, modname .. ':ptest', proplist)
local pt = ptest:create()
pt.sub = {
	range:create(12, 34),
	range:create(56, 78),
	range:create(90, 99),
}

local mbuf = memoryio:create()
local mstr = iostream:create(mbuf)
pt.ptype:write(pt, mstr)
mstr = iostream:create(mbuf)
pt = ptest.ptype:read(mstr)
return pt
