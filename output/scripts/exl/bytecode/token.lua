local modname = ...
local object = package.relrequire(modname, 2, 'object')
local token = object:module(modname)
local argbase
local crc32 = require('crc32')
local scalars = require('rs.scalars')

local ops = {}

for i, op in ipairs{
		'add', -- a, b, target
		'ancillary', -- name(string), ...
			-- ancillary "comment", row(int), col(int), file(string), text(string)
		'call', -- function, args(list), results(list)
		'call_method', -- instance, index(local), args(list), results(list)
		'concat', -- a, b, target
		'div', -- a, b, target
		'function', -- args(list of local), body(block), target
		'local_create', -- value, id(local)
		'move', -- source, target
		'mul', -- a, b, target
		'negate', -- a, target
		'newclass', -- parent, classname(string), target
		'newtable', -- target
		'return', -- values
		'sub', -- a, b, result
	} do
	local id = crc32(op)
	ops[id] = op
end

function token:init(it)
	self.op = it.op
	self.opid = crc32(self.op)
	assert(ops[self.opid])
	self.args = {}
	for i, arg in ipairs(it.args) do
		self.args[i] = argbase:createarg(arg)
	end
end

function token:write(stream)
	scalars.int32:write(self.opid, stream)
	scalars.int:write(#self.args, stream)
	for i = 1, #self.args do
		self.args[i]:writearg(stream)
	end
end

function token:read(stream)
	local instance = self:new()
	instance.opid = scalars.int32:read(stream)
	instance.op = ops[instance.opid] or error('invalid stream')
	instance.args = {}
	local argc = scalars.int:read(stream)
	for i = 1, argc do
		instance.args[i] = argbase:readarg(stream)
	end
end

function token:defstring(lp)
	local argstr = {}
	for i, arg in ipairs(self.args) do
		argstr[i] = arg:defstring(lp)
	end
	argstr = table.concat(argstr, ', ')
	return string.format('%s %s', self.op, argstr)
end

argbase = package.relrequire(modname, 1, 'arg.base')
