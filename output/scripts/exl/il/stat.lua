local modname = ...
local stat = package.modtable(modname)
local common = require(modname, 2, 'common')

stat.ops = {}

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
	stat.ops[opinfo.name] = opinfo
end

function stat.argdefstring(self, lp, tab)
	if type(self) == 'string' then
		return string.format('~%q', self)
	elseif type(self) == 'number' then
		return string.format('~%s', common.dtos(self))
	elseif self[1] == 'block' then
		return string.format('{\n%s\n%s}',
			stat.bodydefstring(self[2], lp .. tab, tab), lp)
	elseif self[1] == 'function' then
		return string.format('.function[%s] {\n%s\n%s}',
			table.concat(self[2], ', '),
			stat.bodydefstring(self[3], lp .. tab, tab), lp)
	elseif self[1] == 'local' then
		return common.identstring(self[2])
	elseif self[1] == 'member' then
		return string.format('%s.%s',
			stat.argdefstring(self[2], lp .. tab, tab),
			common.identstring(self[3]))
	elseif self[1] == 'number' then
		return common.dtos(self[2])
	elseif self[1] == 'ssa' then
		if self[2] == 0 then
			return '.nil'
		else
			return string.format('.%i', self[2])
		end
	elseif self[1] == 'string' then
		return string.format('%q', self[2])
	elseif self[1] == 'list' then
		local itemstr = {}
		for i = 2, #self do
			itemstr[i-1] = stat.argdefstring(self[i], lp, tab)
		end
		itemstr = table.concat(itemstr, ', ')
		return string.format('[%s]', itemstr)
	end
end

function stat.statdefstring(self, lp, tab)
	local argstr = {}
	for i = 2, #self do
		argstr[i-1] = stat.argdefstring(self[i], lp, tab)
	end
	argstr = table.concat(argstr, ', ')
	return string.format('%s %s', self[1], argstr)
end

function stat.bodydefstring(self, lp, tab)
	local lines = {}
	for i, st in ipairs(self) do
		lines[i] = lp .. stat.statdefstring(st, lp, tab)
	end
	return table.concat(lines, '\n')
end
