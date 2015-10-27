local databuffer = require('host.databuffer')
local ffi = require('ffi')
local fontrenderer = require('graphics.fontrenderer')
local pngptype = require('rs.pngptype')
local rs = require('rs')
local util = require('rs.util')

local str = 'qwer 悪いな稗田さんだぜ☆～　（°Д°）'

local function main()
	local font = rs.load('font', 'basefont')
	local ri = fontrenderer.getlinedims(font, str)
	ri.basex = ri.underhang
	ri.basey = ri.descent
	ri.value = 0xff000000
	ri.mask = 1
	local bm = {
		width = ri.width,
		height = ri.height,
	}
	local bmsize = bm.width * bm.height * 4
	bm.db = databuffer:create(bmsize, bmsize)
	ri.bitmap = bm.db:getdata()
	ri.stride = bm.width
	ffi.fill(ri.bitmap, bmsize, 0xff)
	print(ri)
	fontrenderer.render(font, str, ri)
	util.saveptype(pngptype, bm, 'output.png')
end

print(pcall(main))
