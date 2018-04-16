local modname = ...
local types = package.modtable(modname)
local object = require('object')



local function conststr(isconst)
	return isconst and ' const' or ''
end



types.base = object:derive('types.base')

types.base.isconst = false
types.base.isvoid = false

function types.base:sourcetype()
	return nil
end

function types.base:commontype()
	return nil
end

function types.base:sourceargtype()
	return self:sourcetype()
end

function types.base:sourcerettype()
	return self:sourcetype()
end

function types.base:sourcefieldtype()
	return self:sourcetype()
end

function types.base:commonargtype()
	return self:commontype()
end

function types.base:commonrettype()
	return self:commontype()
end

function types.base:commonfieldtype()
	return self:commontype()
end

function types.base:isimmutable()
	return self.isconst
end

function types.base:sourceargprefix()
	return nil
end

function types.base:sourceargsuffix()
	return nil
end

function types.base:sourceretprefix()
	return nil
end

function types.base:sourceretsuffix()
	return nil
end

function types.base:luaargprefix()
	return nil
end

function types.base:luaargsuffix()
	return nil
end

function types.base:luaretprefix()
	return nil
end

function types.base:luaretsuffix()
	return nil
end



types.fundamental = types.base:derive('types.fundamental')

function types.fundamental:init(name, isconst)
	self.name = name
	self.isconst = isconst
	self.isvoid = name == 'void'
end

function types.fundamental:sourcetype()
	return {
		name = self.name,
		isconst = self.isconst}
end

function types.fundamental:commontype()
	return {
		name = self.name,
		isconst = self.isconst}
end

function types.fundamental:defaultvalue()
	return string.format('%s{}',
		self.name)
end

function types.fundamental:serialize()
	return string.format(
		'types.fundamental:create(%q, %s)',
		self.name, self.isconst)
end

function types.fundamental:defstring()
	return string.format(
		'%s%s',
		self.name, conststr(self.isconst))
end



types.namedtype = types.base:derive('types.namedtype')

function types.namedtype:init(decl, isconst)
	self.decl = decl
	self.isconst = isconst
end

function types.namedtype:sourcetype()
	return {
		name = self.decl.sourcename,
		isconst = self.isconst}
end

function types.namedtype:commontype()
	return {
		name = self.decl.commonname,
		isconst = self.isconst}
end

function types.namedtype:defaultvalue()
	return string.format('%s{}',
		self.decl.sourcename)
end

function types.namedtype:defstring()
	return string.format(
		'%s%s',
		self.decl.sourcename, conststr(self.isconst))
end



types.scalartype = types.namedtype:derive('types.scalartype')

function types.scalartype:serialize()
	return string.format(
		'types.scalartype:create(nslookup(moduledef, %q), %s)',
		self.decl.sourcename, self.isconst)
end



types.structtype = types.namedtype:derive('types.structtype')

function types.structtype:serialize()
	return string.format(
		'types.structtype:create(nslookup(moduledef, %q), %s)',
		self.decl.sourcename, self.isconst)
end



types.enumtype = types.namedtype:derive('types.enumtype')

function types.enumtype:commontype()
	return {
		name = self.decl.enumbase,
		isconst = self.isconst}
end

function types.enumtype:serialize()
	return string.format(
		'types.enumtype:create(nslookup(moduledef, %q), %s)',
		self.decl.sourcename, self.isconst)
end



types.pointertype = types.base:derive('types.pointertype')

function types.pointertype:init(target, isconst)
	self.target = target
	self.isconst = isconst
end

function types.pointertype:sourcetype()
	local target = self.target:sourcefieldtype()
	if target then
		return {
			pointer = target,
			isconst = self.isconst}
	end
end

function types.pointertype:commontype()
	local target = self.target:commonfieldtype()
	if target then
		return {
			pointer = target,
			isconst = self.isconst}
	end
end

function types.pointertype:defaultvalue()
	return 'nullptr'
end

function types.pointertype:serialize()
	return string.format(
		'types.pointertype:create(%s, %s)',
		self.target:serialize(), self.isconst)
end

function types.pointertype:defstring()
	return string.format(
		'%s*%s',
		self.target:defstring(), conststr(self.isconst))
end



types.arraytype = types.base:derive('types.arraytype')

function types.arraytype:init(target, size)
	self.target = target
	self.size = size
end

function types.arraytype:sourcetype()
	local target = self.target:sourcefieldtype()
	if target then
		return {
			pointer = target,
			isconst = true}
	end
end

function types.arraytype:sourcefieldtype()
	local target = self.target:sourcefieldtype()
	if target then
		return {
			array = target,
			size = self.size}
	end
end

function types.arraytype:commontype()
	local target = self.target:commonfieldtype()
	if target then
		return {
			pointer = target,
			isconst = true}
	end
end

function types.arraytype:commonfieldtype()
	local target = self.target:commonfieldtype()
	if target then
		return {
			array = target,
			size = self.size}
	end
end

function types.arraytype:isimmutable()
	return true
end

function types.arraytype:defaultvalue()
	return 'nullptr'
end

function types.arraytype:serialize()
	return string.format(
		'types.arraytype:create(%s, %i)',
		self.target:serialize(), self.size)
end

function types.arraytype:defstring()
	return string.format(
		'%s[%i]',
		self.target:defstring(), self.size)
end



types.classboxtype = types.base:derive('types.classboxtype')

function types.classboxtype:init(decl, isconst, istargetconst)
	self.decl = decl
	self.isconst = isconst
	self.istargetconst = istargetconst
end

function types.classboxtype:sourceargtype()
	return {
		pointer = {
			name = self.decl.sourcename,
			isconst = self.istargetconst},
		isconst = self.isconst}
end

function types.classboxtype:sourcerettype()
	return {
		name = 'r::objectbox',
		isconst = self.isconst}
end

function types.classboxtype:commonargtype()
	return {
		pointer = {
			name = self.decl.commonname,
			isconst = self.istargetconst},
		isconst = self.isconst}
end

function types.classboxtype:commonrettype()
	return {
		name = 'r_' .. self.decl.commonname .. '_box'}
end

function types.classboxtype:luaargprefix()
	return 'r_unwrapobject('
end

function types.classboxtype:luaargsuffix()
	return string.format(', %q)', '#'..self.decl.luaname)
end

function types.classboxtype:luaretprefix()
	return 'r_wrapobject('
end

function types.classboxtype:luaretsuffix()
	return ')'
end

function types.classboxtype:defaultvalue()
	return 'r::objectbox{nullptr}'
end



types.classpointertype = types.classboxtype:derive('types.classpointertype')

function types.classpointertype:sourceretprefix()
	return 'r::maketrivialbox('
end

function types.classpointertype:sourceretsuffix()
	return ')'
end

function types.classpointertype:serialize()
	return string.format(
		'types.classpointertype:create(nslookup(moduledef, %q), %s, %s)',
		self.decl.sourcename, self.isconst, self.istargetconst)
end

function types.classpointertype:defstring()
	return string.format(
		'%s%s*%s',
		self.decl.sourcename, conststr(self.isconst),
		conststr(self.istargetconst))
end



types.classreftype = types.classboxtype:derive('types.classreftype')

function types.classreftype:sourceargprefix()
	return 'Ref<' .. self.decl.sourcename .. '>('
end

function types.classreftype:sourceargsuffix()
	return ')'
end

function types.classreftype:sourceretprefix()
	return 'r::makerefbox('
end

function types.classreftype:sourceretsuffix()
	return ')'
end

function types.classreftype:serialize()
	return string.format(
		'types.classreftype:create(nslookup(moduledef, %q), %s, %s)',
		self.decl.sourcename, self.isconst, self.istargetconst)
end

function types.classreftype:defstring()
	return string.format(
		'Ref< %s%s >%s',
		self.decl.sourcename, conststr(self.isconst),
		conststr(self.istargetconst))
end



local function typeobject_trivial(interntype)
	if interntype.fundamental then
		return types.fundamental:create(
			interntype.fundamental, interntype.isconst)
	elseif interntype.pointer then
		local target = typeobject_trivial(interntype.pointer)
		if target then
			return types.pointertype:create(target, interntype.isconst)
		end
	elseif interntype.array then
		local target = typeobject_trivial(interntype.array)
		if target then
			return types.arraytype:create(target, interntype.size)
		end
	elseif interntype.decl then
		local decl = interntype.decl
		if decl.scalartype then
			return types.scalartype:create(decl, interntype.isconst)
		elseif decl.structtype then
			return types.structtype:create(decl, interntype.isconst)
		elseif decl.enumtype then
			return types.enumtype:create(decl, interntype.isconst)
		end
	end
end

function types.typeobject(interntype)
	if interntype.pointer then
		local decl = interntype.pointer.decl
		if decl and decl.classtype then
			return types.classpointertype:create(
				decl, interntype.isconst, interntype.pointer.isconst)
		end
	elseif interntype.template then
		if interntype.template == '::Ref' and #interntype.args == 1 then
			local target = interntype.args[1]
			if target.decl and target.decl.classtype then
				return types.classreftype:create(
					target.decl, interntype.isconst, target.isconst)
			end
		end
	end
	return typeobject_trivial(interntype)
end
