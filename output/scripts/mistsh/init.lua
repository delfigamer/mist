local modname = ...
module(modname, package.seeall)
local peg = require('peg')
local common = require('mistsh.common')

local syntax = peg.loadfile(_PATH .. 'scripts/mistsh/syntax.peg')

function translate(source)
	local stream = peg.stream:create(source)
	local er, ec, tree = syntax:invoke(stream)
	local node = common.createnode(tree)
	local vsh = node:buildvertexshader()
	local fsh = node:buildfragmentshader()
	local tl = node:buildtexturelist()
	return {
		vertex = vsh,
		fragment = fsh,
		texnames = tl,
	}
end
