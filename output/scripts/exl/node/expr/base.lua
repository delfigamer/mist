local modname = ...
local node = require('exl.node')
local ebase = node:module(modname)
local common
local fulltype

local ebasefulltype

function ebase:init(pr)
	node.init(self, pr)
end

function ebase:getfulltype()
	return ebasefulltype
end

function ebase:getconstvalue()
	return nil
end

function ebase:islvalue()
	return false
end

function ebase:isrvalue()
	return true
end

function ebase:gettivalue()
end

common = require('exl.common')
fulltype = require('exl.fulltype')

ebasefulltype = fulltype:create(nil, false, true)
