local modname = ...
local node = require('mistsh.node')
local body = node:module(modname)
local common = require('mistsh.common')

local function buildlist(pr, context, stage)
	local list = {}
	for i, stat in ipairs(pr) do
		list[i] = common.createnode(stat, context)
	end
	return list, innercontext
end

function body:init(pr)
	node.init(self, pr)
	self.context = {
		varyings = {},
		consts = {},
		textures = {},
	}
	self.commoncontex = {
		varyings = self.context.varyings,
		consts = self.context.consts,
		stage = 'common',
	}
	self.vertexcontext = {
		varyings = self.context.varyings,
		textures = self.context.textures,
		attributevars = {},
		uniforms = {},
		attributes = {
			a_position = false,
			a_texcoord1 = false,
			a_texcoord2 = false,
			a_color = false,
		},
		stage = 'vertex',
	}
	self.fragmentcontext = {
		varyings = self.context.varyings,
		textures = self.context.textures,
		attributevars = {},
		uniforms = {},
		stage = 'fragment',
	}
	self.commonpart = buildlist(pr._common, self.commoncontex, 'common')
	self.vertexpart = buildlist(pr._vertex, self.vertexcontext, 'vertex')
	self.vertexreturn = common.createnode(pr._vertexreturn, self.vertexcontext)
	self.fragmentpart = buildlist(pr._fragment, self.fragmentcontext, 'fragment')
	self.fragmentreturn = common.createnode(pr._fragmentreturn, self.fragmentcontext)
end

function body:defstring(lp)
	return
		common.listdefstring(self.commonpart, lp) ..
		'\n\n' .. lp .. 'vertex' ..
		common.listdefstring(self.vertexpart, lp .. self.lpindent) ..
		'\n' .. lp .. self.lpindent .. self.vertexreturn:defstring(lp .. self.lpindent) ..
		'\n\n' .. lp .. 'fragment' ..
		common.listdefstring(self.fragmentpart, lp .. self.lpindent) ..
		'\n' .. lp .. self.lpindent .. self.fragmentreturn:defstring(lp .. self.lpindent) ..
		'\n\n' .. lp .. 'common vars:' ..
		common.listdefstring(self.context.varyings, lp .. self.lpindent) ..
		common.listdefstring(self.context.consts, lp .. self.lpindent) ..
		'\n\n' .. lp .. 'vertex vars:' ..
		common.listdefstring(self.vertexcontext.uniforms, lp .. self.lpindent) ..
		(self.vertexcontext.attributes.a_position and ('\n' .. lp .. self.lpindent .. 'attribute a_position') or '') ..
		(self.vertexcontext.attributes.a_texcoord1 and ('\n' .. lp .. self.lpindent .. 'attribute a_texcoord1') or '') ..
		(self.vertexcontext.attributes.a_texcoord2 and ('\n' .. lp .. self.lpindent .. 'attribute a_texcoord2') or '') ..
		(self.vertexcontext.attributes.a_color and ('\n' .. lp .. self.lpindent .. 'attribute a_color') or '')
		'\n\n' .. lp .. 'fragment vars:' ..
		common.listdefstring(self.fragmentcontext.uniforms, lp .. self.lpindent)
end

if common.target == 'hlsl_3_0' then
	function body:buildvertexshader()
		local posa = self.vertexcontext.attributes.a_position
		local t1a = self.vertexcontext.attributes.a_texcoord1
		local t2a = self.vertexcontext.attributes.a_texcoord2
		local cola = self.vertexcontext.attributes.a_color
		return
			'// built from a mistsh source' ..
			'\nfloat4x4 worldviewmatrix;' ..
			'\nstruct mistsh_varyings_t' ..
			'\n{' ..
			common.listbuildstring(self.context.varyings, 'vheader') ..
			'\n};' ..
			common.listbuildstring(self.context.consts, 'vheader') ..
			common.listbuildstring(self.vertexcontext.uniforms, 'vheader') ..
			'\nstruct mistsh_attributes_t' ..
			'\n{' ..
			(posa and '\nfloat4 a_position: POSITION;' or '') ..
			(t1a and '\nfloat4 a_texcoord1: TEXCOORD0;' or '') ..
			(t2a and '\nfloat4 a_texcoord2: TEXCOORD1;' or '') ..
			(cola and '\nfloat4 a_color: COLOR0;' or '') ..
			'\n};' ..
			'\nfloat4 main(' ..
			'\n\tin mistsh_attributes_t mistsh_attributes,' ..
			'\n\tout mistsh_varyings_t mistsh_varyings )' ..
			'\n\t: POSITION' ..
			'\n{' ..
			common.listbuildstring(self.vertexpart, 'vbody') ..
			'\n' .. self.vertexreturn:build('vbody') ..
			'\n}'
	end
	function body:buildfragmentshader()
		return
			'// built from a mistsh source' ..
			'\nstruct mistsh_varyings_t' ..
			'\n{' ..
			common.listbuildstring(self.context.varyings, 'fheader') ..
			'\n};' ..
			common.listbuildstring(self.context.consts, 'fheader') ..
			common.listbuildstring(self.fragmentcontext.uniforms, 'fheader') ..
			'\nfloat4 main(' ..
			'\n\tin mistsh_varyings_t mistsh_varyings )' ..
			'\n\t: COLOR0' ..
			'\n{' ..
			common.listbuildstring(self.fragmentpart, 'fbody') ..
			'\n' .. self.fragmentreturn:build('fbody') ..
			'\n}'
	end
elseif common.target == 'glsl_100' then
	function body:buildvertexshader()
		local posa = self.vertexcontext.attributes.a_position
		local t1a = self.vertexcontext.attributes.a_texcoord1
		local t2a = self.vertexcontext.attributes.a_texcoord2
		local cola = self.vertexcontext.attributes.a_color
		return
			'#version 100' ..
			'\n// built from a mistsh source' ..
			'\nmat4 worldviewmatrix;' ..
			common.listbuildstring(self.context.varyings, 'vheader') ..
			common.listbuildstring(self.context.consts, 'vheader') ..
			common.listbuildstring(self.vertexcontext.uniforms, 'vheader') ..
			(posa and '\nattribute vec4 a_position;' or '') ..
			(t1a and '\nattribute vec4 a_texcoord1;' or '') ..
			(t2a and '\nattribute vec4 a_texcoord2;' or '') ..
			(cola and '\nattribute vec4 a_color;' or '') ..
			'\nvoid main()' ..
			'\n{' ..
			common.listbuildstring(self.vertexpart, 'vbody') ..
			'\n' .. self.vertexreturn:build('vbody') ..
			'\n}'
	end
	function body:buildfragmentshader()
		return
			'#version 100' ..
			'\n// built from a mistsh source' ..
			common.listbuildstring(self.context.varyings, 'fheader') ..
			common.listbuildstring(self.context.consts, 'fheader') ..
			common.listbuildstring(self.fragmentcontext.uniforms, 'fheader') ..
			'\nvoid main()' ..
			'\n{' ..
			common.listbuildstring(self.fragmentpart, 'fbody') ..
			'\n' .. self.fragmentreturn:build('fbody') ..
			'\n}'
	end
end

function body:buildtexturelist()
	local str = ''
	for i = 0, 7 do
		if self.context.textures[i] then
			str = str .. self.context.textures[i].name .. ';'
		else
			str = str .. ';'
		end
	end
	return str
end
