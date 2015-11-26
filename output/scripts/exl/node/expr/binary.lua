local modname = ...
local ebase = require('exl.node.expr.base')
local ebinary = ebase:module(modname)
local common
-- local prototype

local symbols = {
	concat = '..',

	mul = '*',
	div = '/',

	add = '+',
	sub = '-',

	assign = '=',
	adda = '+=',
	suba = '-=',
}

local internal = {
	assign = true,
}

function ebinary:init(pr)
	ebase.init(self, pr)
	self.operator = pr.operator
	self.binternal = internal[pr.operator]
	self.left = pr.left
	self.right = pr.right
end

function ebinary:build(pc)
	if self.left then
		self.left:build(pc)
	end
	if self.right then
		self.right:build(pc)
	end
	if not self.left or not self.right then
		return
	end
	local leftft = self.left:getfulltype()
	local rightft = self.right:getfulltype()
	local opset
	if self.binternal then
		if leftft.ti then
			opset = leftft.ti:getopset()
		end
	else
		opset = pc:getopset()
	end
	if opset then
		self.operatorfunc = opset:resolve(
			self.operator,
			{leftft, rightft})
	end
	if not self.operatorfunc then
		if self.binternal then
			pc.env:log(
				'error',
				string.format('cannot resolve %s::operator%s(%s, %s)',
					common.defstring(leftft.ti),
					symbols[self.operator],
					leftft, rightft),
				self.spos, self.epos)
		else
			pc.env:log(
				'error',
				string.format('cannot resolve operator%s(%s, %s)',
					symbols[self.operator],
					leftft, rightft),
				self.spos, self.epos)
		end
		return
	end
	self.operatorinstance =
		self.operatorfunc:createinstance{
			context = pc,
			self.left,
			self.right}
	if self.operatorinstance then
		self.constvalue = self.operatorinstance:getconstvalue()
	end
end

function ebinary:getfulltype()
	if self.operatorinstance then
		return self.operatorinstance:getfulltype()
	else
		return ebase.getfulltype(self)
	end
end

function ebinary:getconstvalue()
	return self.constvalue
end

function ebinary:lcompile(stream, source)
	if self.operatorinstance then
		return self.operatorinstance:lcompile(stream, source)
	end
end

function ebinary:rcompile(stream)
	if self.constvalue then
		return self.constvalue:rcompile(stream)
	elseif self.operatorinstance then
		return self.operatorinstance:rcompile(stream)
	end
end

function ebinary:defstring(lp)
	-- if self.constvalue then
		-- return common.defstring(self.constvalue, lp .. self.lpindent)
	-- else
		return string.format('(%s %s %s)',
			common.defstring(self.left, lp .. self.lpindent),
			symbols[self.operator],
			common.defstring(self.right, lp .. self.lpindent))
	-- end
end

common = require('exl.common')
-- prototype = require('exl.prototype')
