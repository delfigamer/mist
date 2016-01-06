local modname = ...
local context = package.relrequire(modname, 2, 'context')
local scontext = context:create()
package.modtable(modname, scontext)
local common = package.relrequire(modname, 2, 'common')
local env = package.relrequire(modname, 2, 'env')
local etypedef = package.relrequire(modname, 2, 'node.expr.typedef')
local fulltype = package.relrequire(modname, 2, 'fulltype')
local nativeof = package.relrequire(modname, 2, 'node.operator.native.factory')
local numberti = package.relrequire(modname, 2, 'node.expr.number').fulltype.ti
local stringti = package.relrequire(modname, 2, 'node.expr.string').fulltype.ti
local symconst = package.relrequire(modname, 2, 'symbol.const')
local utility = require('base.utility')

scontext.env = env:create()

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

local function binopconstf(name, valf)
	return function(args)
		local node = common.createnode{
			name = name,
			spos = args[1].spos,
			epos = args[2].epos,
			filename = args[1].filename,
			value = valf(args[1].value, args[2].value),
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

scontext:setop(
	'concat',
	{rvpt(stringti), rvpt(stringti)},
	nativeof:create{
		rettype = rvpt(stringti),
		constfunc = binopconstf('expr.string', utility.operator.concat),
		opcode = 'concat',
	})

scontext:setop(
	'mul',
	{rvpt(numberti), rvpt(numberti)},
	nativeof:create{
		rettype = rvpt(numberti),
		constfunc = binopconstf('expr.number', utility.operator.mul),
		opcode = 'mul',
	})

scontext:setop(
	'div',
	{rvpt(numberti), rvpt(numberti)},
	nativeof:create{
		rettype = rvpt(numberti),
		constfunc = binopconstf('expr.number', utility.operator.div),
		opcode = 'div',
	})

scontext:setop(
	'add',
	{rvpt(numberti), rvpt(numberti)},
	nativeof:create{
		rettype = rvpt(numberti),
		constfunc = binopconstf('expr.number', utility.operator.add),
		opcode = 'add',
	})

scontext:setop(
	'sub',
	{rvpt(numberti), rvpt(numberti)},
	nativeof:create{
		rettype = rvpt(numberti),
		constfunc = binopconstf('expr.number', utility.operator.sub),
		opcode = 'sub',
	})
