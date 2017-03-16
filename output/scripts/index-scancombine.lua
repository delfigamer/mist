local modname = ...
local index = package.modtable(modname)
local databuffer = require('host.databuffer')
local ffi = require('ffi')
local fileio = require('rs.fileio')
local invoke = require('base.invoke')
local iostream = require('rs.iostream')
local pngptype = require('rs.pngptype')
local rsutil = require('rs.util')

local function ca_to_cb(x,y)
	return
		578.8853924904366 + 0.9956797867445296*x - 0.00008768061734680477*y,
		-34.62794032321968 - 0.0047510131550139445*x + 1.005899633109944*y
end

local function ca_to_cc(x,y)
	return
		325.8386469607249 + 0.9997782819239367*x - 0.004186588391085406*y,
		314.30967699941664 + 0.004894967538444497*x + 1.000008714798765*y
end

local function ca_to_cd(x,y)
	return
		51.31009635205536 + 0.9950578655875046*x + 0.000282156112460542*y,
		335.23393037362234 - 0.004303205547503413*x + 1.0057793623747506*y
end

local default = ffi.new('uint8_t[4]', 255, 255, 255, 255)
local function bmget(bm, x, y)
	x = math.floor(x + 0.5)
	y = bm.height - 1 - math.floor(y - 0.5)
	if x < 0 or x >= bm.width or y < 0 or y >= bm.height then
		return default
	else
		return bm.ptr + bm.width*4*y + 4*x
	end
end

local ca = rsutil.loadptype(pngptype, 'desktop/ca.png')
print('ca loaded')
local cb = rsutil.loadptype(pngptype, 'desktop/cb.png')
print('cb loaded')
local cc = rsutil.loadptype(pngptype, 'desktop/cc.png')
print('cc loaded')
local cd = rsutil.loadptype(pngptype, 'desktop/cd.png')
print('cd loaded')
local cr = {
	width = ca.width,
	height = ca.height,
}
cr.db = databuffer:create(cr.width*cr.height*4, cr.width*cr.height*4)
ca.ptr = ffi.cast('uint8_t*', ca.db:getdata())
cb.ptr = ffi.cast('uint8_t*', cb.db:getdata())
cc.ptr = ffi.cast('uint8_t*', cc.db:getdata())
cd.ptr = ffi.cast('uint8_t*', cd.db:getdata())
cr.ptr = ffi.cast('uint8_t*', cr.db:getdata())

local function pixelcombine(ap, bp, cp, dp)
	local r = math.min(ap[0], bp[0], cp[0], dp[0])
	local g = math.min(ap[1], bp[1], cp[1], dp[1])
	local b = math.min(ap[2], bp[2], cp[2], dp[2])
	return r, g, b
end

for y = 0, cr.height-1 do
	for x = 0, cr.width-1 do
		local cax, cay = x, y
		local cbx, cby = ca_to_cb(cax, cay)
		local ccx, ccy = ca_to_cc(cax, cay)
		local cdx, cdy = ca_to_cd(cax, cay)
		local crp = bmget(cr, x, y)
		local cap = bmget(ca, cax, cay)
		local cbp = bmget(cb, cbx, cby)
		local ccp = bmget(cc, ccx, ccy)
		local cdp = bmget(cd, cdx, cdy)
		local r, g, b = pixelcombine(cap, cbp, ccp, cdp)
		crp[0] = r
		crp[1] = g
		crp[2] = b
		crp[3] = 255
	end
end

print('cr formed')
rsutil.saveptype(pngptype, cr, 'desktop/cr.png')
