local modname = ...
local ref = require('host.ref')
local pixelprogram = ref:module(modname, 'pixelprogram', 'PixelProgram')
local ffi = require('ffi')
local host = require('host')
local ml = require('host.methodlist').graphics

function pixelprogram:create()
	local ptr = ml.pixelprogram_new()
	if ptr ~= nil then
		return self:new(ptr)
	else
		host.checkerror()
		return nil
	end
end

function pixelprogram:setstagetexture(stage, texture)
	if not ml.pixelprogram_setstagetexture(self.ptr, stage, texture and texture.ptr) then
		host.checkerror()
	end
end

function pixelprogram:setstageconstant(stage, constant)
	if not ml.pixelprogram_setstageconstant(self.ptr, stage, constant) then
		host.checkerror()
	end
end

function pixelprogram:setstagecolor(stage, op, arg1, arg2)
	if not ml.pixelprogram_setstagecolor(self.ptr, stage, op, arg1, arg2) then
		host.checkerror()
	end
end

function pixelprogram:setstagealpha(stage, op, arg1, arg2)
	if not ml.pixelprogram_setstagealpha(self.ptr, stage, op, arg1, arg2) then
		host.checkerror()
	end
end

function pixelprogram:setstagetexcoord(stage, index)
	if not ml.pixelprogram_setstagetexcoord(self.ptr, stage, index) then
		host.checkerror()
	end
end

function pixelprogram:setstagematrix(stage, index)
	if not ml.pixelprogram_setstagematrix(self.ptr, stage, index) then
		host.checkerror()
	end
end

function pixelprogram:setmatrix(index, matrix)
	if not ml.pixelprogram_setmatrix(self.ptr, index, matrix) then
		host.checkerror()
	end
end

pixelprogram.Op_Disable = 0
pixelprogram.Op_SelectArg1 = 1
pixelprogram.Op_SelectArg2 = 2
pixelprogram.Op_Modulate = 3
pixelprogram.Op_Modulate2x = 4
pixelprogram.Op_Modulate4x = 5
pixelprogram.Op_Add = 6
pixelprogram.Op_AddSigned = 7
pixelprogram.Op_AddSigned2x = 8
pixelprogram.Op_Subtract = 9
pixelprogram.Op_AddSaturated = 10
pixelprogram.Op_BlendDiffuseAlpha = 11
pixelprogram.Op_BlendTextureAlpha = 12
pixelprogram.Op_BlendCurrentAlpha = 13
pixelprogram.Op_DotProduct = 14
pixelprogram.Op_Invalid = 15
pixelprogram.Arg_Constant = 0
pixelprogram.Arg_Current = 1
pixelprogram.Arg_Diffuse = 2
pixelprogram.Arg_Texture = 3
pixelprogram.Arg_Invalid = 4
pixelprogram.TexCoord_Screen = 0
pixelprogram.TexCoord_Tex1 = 1
pixelprogram.TexCoord_Tex2 = 2
pixelprogram.TexCoord_Invalid = 3
pixelprogram.Matrix_Identity = 0
pixelprogram.Matrix_Mat1 = 1
pixelprogram.Matrix_Mat2 = 2
pixelprogram.Matrix_Invalid = 3

