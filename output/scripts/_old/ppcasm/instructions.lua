local modname = ...
module(modname, package.seeall)
local int = require('ppcasm.int')
local _t = int.template

local function arg(pr, ...) end

local bform_template = {
	_t'6u',
	_t'5u',
	_t'5u',
	_t'14s',
	_t'1u',
	_t'1u'}
local dform_template = {
	_t'6u',
	_t'5u',
	_t'5u',
	_t'16'}
local dsform_template = {
	_t'6u',
	_t'5u',
	_t'5u',
	_t'14',
	_t'2u'}
local iform_template = {
	_t'6u',
	_t'24s',
	_t'1u',
	_t'1u'}
local scform_template = {
	_t'6u',
	_t'5',
	_t'5',
	_t'4',
	_t'7u',
	_t'3',
	_t'1u',
	_t'1'}
local xform_template = {
	_t'6u',
	_t'5u',
	_t'5u',
	_t'5u',
	_t'10u',
	_t'1u'}
local xoform_template = {
	_t'6u',
	_t'5u',
	_t'5u',
	_t'5u',
	_t'1u',
	_t'9u',
	_t'1u'}
local xlform_template = {
	_t'6u',
	_t'5u',
	_t'5u',
	_t'3',
	_t'2u',
	_t'10u',
	_t'1'}
local bo_template = {
	_t'1u',
	_t'1u',
	_t'1u',
	_t'1u',
	_t'1u'}
local cr_l_template = {
	_t'3u',
	_t'1',
	_t'1u'}
local long_template = {
	_t'32'}

local args = {}

function args.decint(pr, labels)
	return tonumber(pr._digits) * (pr._sign == '-' and -1 or 1)
end

function args.hexint(pr, labels)
	return tonumber(pr._digits, 16) * (pr._sign == '-' and -1 or 1)
end

function args.binint(pr, labels)
	return tonumber(pr._digits, 2) * (pr._sign == '-' and -1 or 1)
end

function args.labelref(pr, labels)
	return labels[pr._target] or error('undefined symbol: '..pr._target)
end

function args.rindex(pr, labels)
	return tonumber(pr._index)
end

function args.crindex(pr, labels)
	return tonumber(pr._index)
end

local function boolmnarg(pr, labels)
	return pr[1] and 1 or 0
end

args.aa = boolmnarg
args.lk = boolmnarg
args.oe = boolmnarg
args.rc = boolmnarg

local condition_table = {
	lt = {0, 1},
	le = {1, 0},
	eq = {2, 1},
	ge = {0, 0},
	gt = {1, 1},
	nl = {0, 0},
	ne = {2, 0},
	ng = {1, 0},
	so = {3, 1},
	ns = {3, 0}}

function args.condition(pr, labels)
	local val = condition_table[pr[1]]
	return val[1], val[2]
end

local decrement_table = {
	dz = 1,
	dnz = 0}

function args.decrement(pr, labels)
	return decrement_table[pr[1]]
end

local spr_table = {
	xer = {0, 1},
	lr = {0, 8},
	ctr = {0, 9}}

function args.spr(pr, labels)
	local val = spr_table[pr[1]]
	return val[1], val[2]
end

local l_table = {
	w = 0,
	d = 1}

function args.l(pr, labels)
	return l_table[pr[1]]
end

function args.dec_cond(pr, crpr, labels)
	local b0, b1, b2, b3, bi
	if pr._decrement then
		b2, b3 = 0, arg(pr._decrement, labels)
	else
		b2, b3 = 1, 0
	end
	if pr._condition then
		b0, bi, b1 = 0, arg(pr._condition, labels)
		if crpr then
			bi = bi + 4 * arg(crpr, labels)
		end
	else
		b0, bi, b1 = 1, 0, 0
	end
	return int.bitfield(bo_template, b0, b1, b2, b3, 0), bi
end

function arg(pr, ...)
	return args[pr.name](pr, ...)
end

local function i_rrr_arith(opcd, xo)
	return function(pr, labels)
		return int.bitfield(
			xoform_template,
			opcd,
			arg(pr._rt, labels),
			arg(pr._ra, labels),
			arg(pr._rb, labels),
			arg(pr._oe, labels),
			xo,
			arg(pr._rc, labels))
	end
end

local function i_rrr_logic(opcd, xo)
	return function(pr, labels)
		return int.bitfield(
			xform_template,
			opcd,
			arg(pr._ra, labels),
			arg(pr._rt, labels),
			arg(pr._rb, labels),
			xo,
			arg(pr._rc, labels))
	end
end

local function i_lcrr(opcd, xo)
	return function(pr, labels)
		local cr_l = int.bitfield(
			cr_l_template,
			pr._cr and arg(pr._cr, labels) or 0,
			0,
			arg(pr._l, labels))
		return int.bitfield(
			xform_template,
			opcd,
			cr_l,
			arg(pr._ra, labels),
			arg(pr._rb, labels),
			xo,
			0)
	end
end

local function i_rr_logic(opcd, xo)
	return function(pr, labels)
		local ra = arg(pr._ra, labels)
		return int.bitfield(
			xform_template,
			opcd,
			ra,
			arg(pr._rt, labels),
			ra,
			xo,
			arg(pr._rc, labels))
	end
end

local function i_ri(opcd)
	return function(pr, labels)
		return int.bitfield(
			dform_template,
			opcd,
			arg(pr._rt, labels),
			0,
			arg(pr._i, labels))
	end
end

local function i_rri_arith(opcd)
	return function(pr, labels)
		return int.bitfield(
			dform_template,
			opcd,
			arg(pr._rt, labels),
			arg(pr._ra, labels),
			arg(pr._i, labels))
	end
end

local function i_rri_arith_invert(opcd)
	return function(pr, labels)
		return int.bitfield(
			dform_template,
			opcd,
			arg(pr._rt, labels),
			arg(pr._ra, labels),
			-arg(pr._i, labels))
	end
end

local function i_rri_logic(opcd)
	return function(pr, labels)
		return int.bitfield(
			dform_template,
			opcd,
			arg(pr._ra, labels),
			arg(pr._rt, labels),
			arg(pr._i, labels))
	end
end

local function i_lcri(opcd)
	return function(pr, labels)
		local cr_l = int.bitfield(
			cr_l_template,
			pr._cr and arg(pr._cr, labels) or 0,
			0,
			arg(pr._l, labels))
		return int.bitfield(
			dform_template,
			opcd,
			cr_l,
			arg(pr._ra, labels),
			arg(pr._i, labels))
	end
end

local function i_ribase(opcd)
	return function(pr, labels)
		return int.bitfield(
			dform_template,
			opcd,
			arg(pr._rt, labels),
			arg(pr._base, labels),
			arg(pr._i, labels))
	end
end

local function i_ribase_ds(opcd, xo)
	return function(pr, labels)
		local offset = arg(pr._i, labels)
		if bit.band(offset, 3) ~= 0 then
			error('offset must be word-aligned')
		end
		return int.bitfield(
			dsform_template,
			opcd,
			arg(pr._rt, labels),
			arg(pr._base, labels),
			bit.arshift(offset, 2),
			xo)
	end
end

local function i_blai(opcd)
	return function(pr, labels)
		local aa = arg(pr._aa, labels)
		local address = arg(pr._address, labels)
		if aa == 0 then
			address = address - pr.address
		end
		return int.bitfield(
			iform_template,
			opcd,
			bit.arshift(address, 2),
			aa,
			arg(pr._lk, labels))
	end
end

local function i_bdclai(opcd)
	return function(pr, labels)
		local aa = arg(pr._aa, labels)
		local address = arg(pr._address, labels)
		if aa == 0 then
			address = address - pr.address
		end
		local bo, bi = arg(pr._dec_cond, pr._cr, labels)
		return int.bitfield(
			bform_template,
			opcd,
			bo,
			bi,
			_M.bit.arshift(address, 2),
			aa,
			arg(pr._lk, labels))
	end
end

local function i_bdcl(opcd, xo)
	return function(pr, labels)
		local bo, bi = arg(pr._dec_cond, pr._cr, labels)
		return int.bitfield(
			xlform_template,
			opcd,
			bo,
			bi,
			0,
			0,
			xo,
			arg(pr._lk, labels))
	end
end

local function i_sprr(opcd, xo)
	return function(pr, labels)
		local sprh, sprl = arg(pr._spr)
		return int.bitfield(
			xform_template,
			opcd,
			arg(pr._rt, labels),
			sprl,
			sprh,
			xo,
			0)
	end
end

local function i_sc(opcd)
	return function(pr, labels)
		return int.bitfield(
			scform_template,
			opcd,
			0,
			0,
			0,
			pr._lev and arg(pr._lev, labels) or 0,
			0,
			1,
			0)
	end
end

local function i_bl(opcd, xo)
	return function(pr, labels)
		return int.bitfield(
			xlform_template,
			opcd,
			0x14,
			0,
			0,
			0,
			xo,
			arg(pr._lk, labels))
	end
end

addis = i_rri_arith(15)
addi = i_rri_arith(14)
add = i_rrr_arith(31, 266)
bcctr_cond = i_bdcl(19, 528)
bctr = i_bl(19, 528)
bc_cond = i_bdclai(16)
bclr_cond = i_bdcl(19, 16)
blr = i_bl(19, 16)
b = i_blai(18)
cmpi = i_lcri(11)
cmpli = i_lcri(10)
cmp = i_lcrr(31, 0)
cmpl = i_lcrr(31, 32)
lbzu = i_ribase(35)
lbz = i_ribase(34)
ld = i_ribase(58, 0)
ldu = i_ribase(58, 1)
lhau = i_ribase(43)
lha = i_ribase(42)
lhzu = i_ribase(41)
lhz = i_ribase(40)
lis = i_ri(15)
li = i_ri(14)
lwa = i_ribase_ds(58, 2)
lwzu = i_ribase(33)
lwz = i_ribase(32)
mfspr_ex = i_sprr(31, 339)
mr = i_rr_logic(31, 444)
mtspr_ex = i_sprr(31, 467)
oris = i_rri_logic(25)
ori = i_rri_logic(24)
_M['or'] = i_rrr_logic(31, 444)
sc = i_sc(17)
stdu = i_ribase_ds(62, 1)
std = i_ribase_ds(62, 0)
stwu = i_ribase(37)
stw = i_ribase(36)
subis = i_rri_arith_invert(15)
subi = i_rri_arith_invert(14)

_M['.long'] = function(pr, labels)
	return int.bitfield(
		long_template,
		arg(pr._value))
end
