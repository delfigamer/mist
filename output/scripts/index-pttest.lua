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

--[[
	return (attribute 1 * attribute 2) ^ float4(2.2, 2.2, 2.2, 1)
]]
local tokens = {
	pt.attribute_1,
	pt.attribute_2,
	pt.multiply,
	pt.literal_float4, ftoi(2.2), ftoi(2.2), ftoi(2.2), ftoi(1),
	pt.power,
}

local db = databuffer:create(#tokens*4, #tokens*4, nil)
local data = ffi.cast('int*', db:getdata())
for i, token in ipairs(tokens) do
	data[i-1] = token
end
local tt = translatortest:create(db)
