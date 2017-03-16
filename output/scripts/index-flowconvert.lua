local modname = ...
local index = package.modtable(modname)
local databuffer = require('host.databuffer')
local ffi = require('ffi')
local pngptype = require('rs.pngptype')
local rsutil = require('rs.util')

local function clampval(c)
	return (math.abs(c) - math.abs(c-255) + 255) * 0.5
end

local levelmap = {}
for i = 0, 255 do
	local val = (i-110) * (255/(255-110))
	levelmap[i] = clampval(math.floor(0.5 + math.pow(val/255, 0.8)*255))
end

local function levelbm(bm)
	local bmptr = ffi.cast('uint8_t*', bm.db:getdata())
	for y = 0, bm.height-1 do
		local row = bmptr + bm.width*4*y
		for x = 0, bm.width-1 do
			local pixel = row + 4*x
			pixel[0] = levelmap[pixel[0]]
			pixel[1] = levelmap[pixel[1]]
			pixel[2] = levelmap[pixel[2]]
		end
	end
end

local function reducebmpixel(ptr, stride)
	local rsum =
		ptr[         0] + ptr[         4] + ptr[          8] + ptr[         12]
	local gsum =
		ptr[         1] + ptr[         5] + ptr[          9] + ptr[         13]
	local bsum =
		ptr[         2] + ptr[         6] + ptr[         10] + ptr[         14]

	rsum = rsum +
		ptr[  stride+0] + ptr[  stride+4] + ptr[  stride+ 8] + ptr[  stride+12]
	gsum = gsum +
		ptr[  stride+1] + ptr[  stride+5] + ptr[  stride+ 9] + ptr[  stride+13]
	bsum = bsum +
		ptr[  stride+2] + ptr[  stride+6] + ptr[  stride+10] + ptr[  stride+14]

	rsum = rsum +
		ptr[2*stride+0] + ptr[2*stride+4] + ptr[2*stride+ 8] + ptr[2*stride+12]
	gsum = gsum +
		ptr[2*stride+1] + ptr[2*stride+5] + ptr[2*stride+ 9] + ptr[2*stride+13]
	bsum = bsum +
		ptr[2*stride+2] + ptr[2*stride+6] + ptr[2*stride+10] + ptr[2*stride+14]

	rsum = rsum +
		ptr[3*stride+0] + ptr[3*stride+4] + ptr[3*stride+ 8] + ptr[3*stride+12]
	gsum = gsum +
		ptr[3*stride+1] + ptr[3*stride+5] + ptr[3*stride+ 9] + ptr[3*stride+13]
	bsum = bsum +
		ptr[3*stride+2] + ptr[3*stride+6] + ptr[3*stride+10] + ptr[3*stride+14]

	local rval = (1/16)*rsum
	local gval = (1/16)*gsum
	local bval = (1/16)*bsum

	return rval, gval, bval
end

local function reducebm(bm)
	local bmptr = ffi.cast('uint8_t*', bm.db:getdata())
	local newwidth = math.floor(bm.width/4)
	local newheight = math.floor(bm.height/4)
	local oldstride = bm.width*4
	for y = 0, newheight-1 do
		local newrow = bmptr + newwidth*4*y
		local oldrow = bmptr + oldstride*4*y
		for x = 0, newwidth-1 do
			local newpixel = newrow + 4*x
			local oldpixel = oldrow + 4*4*x
			newpixel[0], newpixel[1], newpixel[2] =
				reducebmpixel(oldpixel, oldstride)
		end
	end
	bm.width = newwidth
	bm.height = newheight
	bm.db:setlength(bm.width*bm.height*4)
end

for i, entry in ipairs{
	-- { 0, 'sf_clean/ti.png'},
	{ 1, 'sf/Рисунок (0).png'},
	{ 2, 'sf/Рисунок.png'},
	{ 3, 'sf/Рисунок (2).png'},
	{ 4, 'sf/Рисунок (3).png'},
	{ 5, 'sf/Рисунок (4).png'},
	{ 6, 'sf/Рисунок (5).png'},
	{ 7, 'sf/Рисунок (6).png'},
	{ 8, 'sf/Рисунок (7).png'},
	{ 9, 'sf/Рисунок (8).png'},
	{10, 'sf/Рисунок (9).png'},
	{11, 'sf/Рисунок (10).png'},
	{12, 'sf/Рисунок (11).png'},
	{13, 'sf/Рисунок (12).png'},
	{14, 'sf/Рисунок (13).png'},
	{15, 'sf/Рисунок (14).png'},
	{16, 'sf/Рисунок (15).png'},
	{17, 'sf/Рисунок (16).png'},
	{18, 'sf/Рисунок (17).png'},
	{19, 'sf/Рисунок (18).png'},
	{20, 'sf/Рисунок (19).png'},
	{21, 'sf/Рисунок (20).png'},
	{22, 'sf/Рисунок (21).png'},
	{23, 'sf/Рисунок (22).png'},
	{24, 'sf/Рисунок (23).png'},
	{25, 'sf/Рисунок (24).png'},
	{26, 'sf/Рисунок (25).png'},
	{27, 'sf/Рисунок (26).png'},
	{28, 'sf/Рисунок (27).png'},
	{29, 'sf/Рисунок (28).png'},
	{30, 'sf/Рисунок (29).png'},
	{31, 'sf/Рисунок (30).png'},
	{32, 'sf/Рисунок (31).png'},
	{33, 'sf/Рисунок (32).png'},
	{34, 'sf/Рисунок (33).png'},
	{35, 'sf/Рисунок (34).png'},
	{36, 'sf/Рисунок (35).png'},
	{37, 'sf/Рисунок (36).png'},
	{38, 'sf/Рисунок (37).png'},
	{39, 'sf/Рисунок (38).png'},
	{40, 'sf/Рисунок (39).png'},
	{41, 'sf/Рисунок (40).png'},
	{42, 'sf/Рисунок (41).png'},
	{43, 'sf/Рисунок (42).png'},
	{44, 'sf/Рисунок (43).png'},
	{45, 'sf/Рисунок (44).png'},
	{46, 'sf/Рисунок (45).png'},
	{47, 'sf/Рисунок (46).png'},
	{48, 'sf/Рисунок (47).png'},
	{49, 'sf/Рисунок (48).png'},
	{50, 'sf/Рисунок (49).png'},
	{51, 'sf/Рисунок (50).png'},
	{52, 'sf/Рисунок (51).png'},
	{53, 'sf/Рисунок (52).png'},
	{54, 'sf/Рисунок (53).png'},
	{55, 'sf/Рисунок (54).png'},
	{56, 'sf/Рисунок (55).png'},
	{57, 'sf/Рисунок (56).png'},
	{58, 'sf/Рисунок (57).png'},
	{59, 'sf/Рисунок (58).png'},
	{60, 'sf/Рисунок (59).png'},
	{61, 'sf/Рисунок (63).png'},
	{62, 'sf/Рисунок (62).png'},
	{63, 'sf/Рисунок (61).png'},
	{64, 'sf/Рисунок (60).png'},
	{65, 'sf/Рисунок (74).png'},
	{66, 'sf/Рисунок (73).png'},
	{67, 'sf/Рисунок (72).png'},
	{68, 'sf/Рисунок (71).png'},
	{69, 'sf/Рисунок (70).png'},
	{70, 'sf/Рисунок (69).png'},
	{71, 'sf/Рисунок (68).png'},
	{72, 'sf/Рисунок (67).png'},
	{73, 'sf/Рисунок (66).png'},
	{74, 'sf/Рисунок (65).png'},
	{75, 'sf/Рисунок (64).png'},
} do
	local index, filename = entry[1], entry[2]
	print('import:', filename)
	local bm = rsutil.loadptype(pngptype, filename)
	print('level')
	levelbm(bm)
	print('reduce')
	reducebm(bm)
	local retname = string.format('sf_clean/i%.3i.png', index)
	print('export:', retname)
	rsutil.saveptype(pngptype, bm, retname)
	bm.db:release()
end
