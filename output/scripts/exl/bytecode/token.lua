local modname = ...
local object = package.relrequire(modname, 2, 'object')
local token = object:module(modname)
local argbase
local crc32 = require('crc32')
local scalars = require('rs.scalars')

local ops = {}

for i, opinfo in ipairs{
		{
			name = 'add',
			args = {
				{
					name = 'a',
					role = 'input',
					type = {},
				},
				{
					name = 'b',
					role = 'input',
					type = {},
				},
				{
					name = 'result',
					role = 'output',
					type = {},
				},
			},
		},
		{
			name = 'ancillary',
			args = {
			},
			-- ancillary "comment", row(int), col(int), file(string), text(string)
			-- ancillary
		},
		{
			name = 'call',
			args = {
				{
					name = 'function',
					role = 'input',
					type = {},
				},
				{
					name = 'args',
					role = 'input',
					type = {'list'},
				},
				{
					name = 'results',
					role = 'output',
					type = {'list'},
				},
			},
		},
		{
			name = 'call_method',
			args = {
				{
					name = 'object',
					role = 'input',
					type = {},
				},
				{
					name = 'name',
					role = 'input',
					type = {'local'},
				},
				{
					name = 'args',
					role = 'input',
					type = {'list'},
				},
				{
					name = 'results',
					role = 'output',
					type = {'list'},
				},
			},
		},
		{
			name = 'concat',
			args = {
				{
					name = 'a',
					role = 'input',
					type = {},
				},
				{
					name = 'b',
					role = 'input',
					type = {},
				},
				{
					name = 'result',
					role = 'output',
					type = {},
				},
			},
		},
		{
			name = 'div',
			args = {
				{
					name = 'a',
					role = 'input',
					type = {},
				},
				{
					name = 'b',
					role = 'input',
					type = {},
				},
				{
					name = 'result',
					role = 'output',
					type = {},
				},
			},
		},
		{
			name = 'instantiate',
			args = {
				{
					name = 'class',
					role = 'input',
					type = {},
				},
				{
					name = 'target',
					role = 'output',
					type = {},
				},
			},
		},
		{
			name = 'local_create',
			args = {
				{
					name = 'value',
					role = 'input',
					type = {},
				},
				{
					name = 'id',
					role = 'output',
					type = {'local'},
				},
			},
		},
		{
			name = 'move',
			args = {
				{
					name = 'source',
					role = 'input',
					type = {},
				},
				{
					name = 'target',
					role = 'output',
					type = {},
				},
			},
		},
		{
			name = 'mul',
			args = {
				{
					name = 'a',
					role = 'input',
					type = {},
				},
				{
					name = 'b',
					role = 'input',
					type = {},
				},
				{
					name = 'result',
					role = 'output',
					type = {},
				},
			},
		},
		{
			name = 'negate',
			args = {
				{
					name = 'a',
					role = 'input',
					type = {},
				},
				{
					name = 'result',
					role = 'output',
					type = {},
				},
			},
		},
		{
			name = 'newclass',
			args = {
				{
					name = 'parent',
					role = 'input',
					type = {},
				},
				{
					name = 'classname',
					role = 'input',
					type = {'string'},
				},
				{
					name = 'target',
					role = 'output',
					type = {},
				},
			},
		},
		{
			name = 'return',
			args = {
				{
					name = 'values',
					role = 'input',
					type = {'list'},
				},
			},
		},
		{
			name = 'sub',
			args = {
				{
					name = 'a',
					role = 'input',
					type = {},
				},
				{
					name = 'b',
					role = 'input',
					type = {},
				},
				{
					name = 'result',
					role = 'output',
					type = {},
				},
			},
		},
	} do
	opinfo.id = crc32(opinfo.name)
	ops[opinfo.name] = opinfo
	ops[opinfo.id] = opinfo
end

function token:init(it)
	self.opinfo = assert(ops[it.op])
	self.args = {}
	for i, arg in ipairs(it.args) do
		self.args[i] = argbase:createarg(arg)
	end
end

function token:write(stream)
	scalars.int32:write(self.opinfo.id, stream)
	scalars.int:write(#self.args, stream)
	for i = 1, #self.args do
		self.args[i]:writearg(stream)
	end
end

function token:read(stream)
	local instance = self:new()
	local opid = scalars.int32:read(stream)
	instance.opinfo = ops[opid] or error('invalid stream')
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
	return string.format('%s %s', self.opinfo.name, argstr)
end

argbase = package.relrequire(modname, 1, 'arg.base')
