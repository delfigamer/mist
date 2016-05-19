local modname = ...
local message = require('game.message.base')
local setlocation = message:module(modname, 3, 'setlocation')

setlocation.fields = message.fields .. 'float x;float y;float yaw;'
