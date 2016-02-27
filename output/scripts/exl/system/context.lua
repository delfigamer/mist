local modname = ...
local context = require(modname, 2, 'context')
local scontext = context:create()
package.modtable(modname, scontext)
local common = require(modname, 2, 'common')
local env = require(modname, 2, 'env')
local etypedef = require(modname, 2, 'node.expr.typedef')
local fulltype = require(modname, 2, 'fulltype')
local nativeof = require(modname, 2, 'node.operator.native.operator')
local numberti = require(modname, 2, 'node.expr.number').fulltype.ti
local stringti = require(modname, 2, 'node.expr.string').fulltype.ti
local symconst = require(modname, 2, 'symbol.const')
local utility = require('base.utility')

scontext.env = env:create()
scontext.nameprefix = 'system'

scontext:setsymbol('number', symconst:create{
	context = scontext,
	fulltype = fulltype:create(nil, false, false),
	constvalue = etypedef:create{
		typeinfo = numberti,
	},
})

scontext:setsymbol('string', symconst:create{
	context = scontext,
	fulltype = fulltype:create(nil, false, false),
	constvalue = etypedef:create{
		typeinfo = stringti,
	},
})

local function unopconstf(name, valf)
	return function(it)
		local node = common.createnode{
			name = name,
			spos = it.spos,
			epos = it.epos,
			filename = it.filename,
			value = valf(it.args[1].value),
		}
		return node
	end
end

local function binopconstf(name, valf)
	return function(it)
		local node = common.createnode{
			name = name,
			spos = it.spos,
			epos = it.epos,
			filename = it.filename,
			value = valf(it.args[1].value, it.args[2].value),
		}
		return node
	end
end

local function rvpt(ti)
	return fulltype:create(ti, false, true)
end

local function lvpt(ti)
	return fulltype:create(ti, true, false)
end

local function lrvpt(ti)
	return fulltype:create(ti, true, true)
end

table.append(scontext:getoperatorlist('concat'),
	nativeof:create{
		args = {rvpt(stringti), rvpt(stringti)},
		retfulltype = rvpt(stringti),
		constfunc = binopconstf('expr.string', utility.operator.concat),
		opcode = 'concat',
	})

table.append(scontext:getoperatorlist('negate'),
	nativeof:create{
		args = {rvpt(numberti)},
		retfulltype = rvpt(numberti),
		constfunc = unopconstf('expr.number', utility.operator.unm),
		opcode = 'negate',
	})

table.append(scontext:getoperatorlist('mul'),
	nativeof:create{
		args = {rvpt(numberti), rvpt(numberti)},
		retfulltype = rvpt(numberti),
		constfunc = binopconstf('expr.number', utility.operator.mul),
		opcode = 'mul',
	})

table.append(scontext:getoperatorlist('div'),
	nativeof:create{
		args = {rvpt(numberti), rvpt(numberti)},
		retfulltype = rvpt(numberti),
		constfunc = binopconstf('expr.number', utility.operator.div),
		opcode = 'div',
	})

table.append(scontext:getoperatorlist('add'),
	nativeof:create{
		args = {rvpt(numberti), rvpt(numberti)},
		retfulltype = rvpt(numberti),
		constfunc = binopconstf('expr.number', utility.operator.add),
		opcode = 'add',
	})

table.append(scontext:getoperatorlist('sub'),
	nativeof:create{
		args = {rvpt(numberti), rvpt(numberti)},
		retfulltype = rvpt(numberti),
		constfunc = binopconstf('expr.number', utility.operator.sub),
		opcode = 'sub',
	})

table.append(scontext:getoperatorlist('assign'),
	require(modname, 2, 'node.operator.assign.operator'))

table.append(scontext:getoperatorlist('init'),
	require(modname, 2, 'node.operator.init.operator'))

table.append(scontext:getoperatorlist('identity'),
	require(modname, 2, 'node.operator.identity.operator'))

table.append(scontext:getoperatorlist('call'),
	require(modname, 2, 'node.operator.functioncall.operator'))
