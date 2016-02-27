local modname = ...
local io = require('host.io')
local packageio = io:module(modname, 'packageio', 'PackageIo', 0x0b396ef5)
local host = require('host')
local ffi = require('ffi')
local ml = require('host.methodlist').rsbin
