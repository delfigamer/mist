local databuffer = require('host.databuffer')
local ffi = require('ffi')
local pt = require('host.programtoken')
local translatortest = require('host.TranslatorTest')

local floatint_t = ffi.typeof[[
	union {
		float f;
		int i;
	}
]]

local function ftoi(n)
	return floatint_t(n).i
end

local tokens = {
	pt.attribute_1,
	pt.attribute_2,
	pt.multiply,
	pt.extract_0,
	pt.power,
	pt.texture_0,
}

local db = databuffer:create(#tokens*4, #tokens*4, nil)
local data = ffi.cast('int*', db:getdata())
for i, token in ipairs(tokens) do
	data[i-1] = token
end
local tt = translatortest:create(db)
