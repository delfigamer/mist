local modname = ...
local fontrenderer = package.modtable(modname)
local ffi = require('ffi')
local utf8 = require('utf8')

fontrenderer.renderinfo_t = ffi.metatype([[
	struct {
		int extraadvance;
		int shearperiod;
		int basex;
		int basey;
		void* bitmap;
		int stride;
		uint32_t value;
		uint32_t mask;
		int height;
		int width;
		int descent;
		int underhang;
		int overhang;
	} ]], {
		__tostring = function(self)
			return string.format(
				'{\n' ..
				'\textraadvance = %i,\n' ..
				'\tshearperiod = %i,\n' ..
				'\tbasex = %i,\n' ..
				'\tbasey = %i,\n' ..
				'\tbitmap = %s,\n' ..
				'\tstride = %i,\n' ..
				'\tvalue = %#.8x,\n' ..
				'\tmask = %#x,\n' ..
				'\theight = %i,\n' ..
				'\twidth = %i,\n' ..
				'\tdescent = %i,\n' ..
				'\tunderhang = %i,\n' ..
				'\toverhang = %i,\n' ..
				'}',
				self.extraadvance,
				self.shearperiod,
				self.basex,
				self.basey,
				self.bitmap,
				self.stride,
				self.value,
				self.mask,
				self.height,
				self.width,
				self.descent,
				self.underhang,
				self.overhang
			)
		end,
	})

function fontrenderer.getlinedims(font, line, extraadvance, shearperiod)
	local ri = fontrenderer.renderinfo_t()
	ri.extraadvance = extraadvance or 0
	ri.shearperiod = shearperiod or 0
	local lineascent = 0
	local linedescent = 0
	local lineleft = 0
	local lineright = 0
	local lineposition = 0
	for p, charcode in utf8.codes(line) do
		local cr = font:getchar(charcode)
		if cr then
			local charascent = cr.height + cr.yoffset
			if charascent > lineascent then
				lineascent = charascent
			end
			local chardescent = -cr.yoffset
			if chardescent > linedescent then
				linedescent = chardescent
			end
			local charleft = -cr.xoffset - lineposition
			if charleft > lineleft then
				lineleft = charleft
			end
			local charright = cr.width + ri.extraadvance - charleft
			if charright > lineright then
				lineright = charright
			end
			lineposition = lineposition + cr.advance + ri.extraadvance
		end
	end
	ri.height = lineascent + linedescent
	ri.width = lineleft + lineright
	ri.descent = linedescent
	ri.underhang = lineleft
	ri.overhang = lineright - lineposition
	if ri.shearperiod ~= 0 then
		local shearw = math.floor(ri.height / ri.shearperiod)
		if shearw > 0 then
			ri.width = ri.width + shearw
			ri.overhang = ri.overhang + shearw
		else
			ri.width = ri.width - shearw
			ri.underhang = ri.underhang - shearw
		end
	end
	return ri
end

local function renderchar(
	cr, bitmap, bmstride, basex, basey,
	extrawidth, shearperiod, shearoffset,
	value, mask
)
	basex = basex + cr.xoffset + extrawidth
	basey = basey + cr.yoffset
	local stride = bit.rshift(cr.width + 7, 3)
	for cy = 0, cr.height-1 do
		local bitmaprow = bitmap + bmstride * (basey + cy) + basex
		if shearperiod ~= 0 then
			bitmaprow = bitmaprow + math.floor(
				(basey + cy + shearoffset) / shearperiod)
		end
		local charrow = cr.data + stride * cy;
		for cx = 0, extrawidth-1 do
			if bit.band(mask,
				bit.rshift(bit.lshift(charrow[0], extrawidth), cx)) ~= 0
			then
				bitmaprow[cx - extrawidth] = value
			end
		end
		for wcx = 0, stride-2 do
			local chmask = bit.bor(charrow[wcx], bit.lshift(charrow[wcx+1], 8))
			for cx = 0, 7 do
				if bit.band(mask, bit.rshift(chmask, cx)) ~= 0 then
					bitmaprow[bit.bor(bit.lshift(wcx, 3), cx)] = value
				end
			end
		end
		for cx = 0, cr.width - bit.lshift(stride, 3) + 7 do
			if bit.band(mask, bit.rshift(charrow[stride-1], cx)) ~= 0 then
				bitmaprow[bit.bor(bit.lshift(stride-1, 3), cx)] = value
			end
		end
	end
end

function fontrenderer.render(font, line, ri)
	local currentx = ri.basex
	local bitmap = ffi.cast('uint32_t*', ri.bitmap)
	for p, charcode in utf8.codes(line) do
		local cr = font:getchar(charcode)
		if cr then
			renderchar(
				cr, bitmap, ri.stride,
				currentx, ri.basey,
				ri.extraadvance, ri.shearperiod, ri.basey - ri.descent,
				ri.value, ri.mask)
			currentx = currentx + cr.advance + ri.extraadvance
		end
	end
end
