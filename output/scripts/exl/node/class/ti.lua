local modname = ...
local typeinfo = require('exl.typeinfo')
local classti = typeinfo:module(modname)
local common

function classti:init(pr)
	typeinfo.init(self, pr)
	self.parent = pr.parent
	self.body = pr.body
end

function functionti:iseq(other)
	if not other['#' .. functionti._NAME] then
		return false
	elseif not self.arglist or not other.arglist then
		return false
	elseif #self.arglist.args ~= #other.arglist.args then
		return false
	elseif (not self.rettype) ~= (not other.rettype) then
		return false
	elseif self.rettype then
		local rti = self.rettype:gettivalue()
		local orti = other.rettype:gettivalue()
		if not rti or not orti or not rti:iseq(orti) then
			return false
		end
	end
	for i, arg in ipairs(self.arglist.args) do
		local oarg = other.arglist.args[i]
		if not arg.typev or not oarg.typev then
			return false
		elseif arg.blvalue ~= oarg.blvalue or arg.brvalue ~= oarg.brvalue then
			return false
		end
		local ati = arg.typev:gettivalue()
		local oti = oarg.typev:gettivalue()
		if not ati or not oti or not ati:iseq(oti) then
			return false
		end
	end
	return true
end

function functionti:getdefaultopfunc(op, proto)
	if op == 'call' then
		return cofunc
	else
		return typeinfo.getdefaultopfunc(self, op, proto)
	end
end

function functionti:defstring(lp)
	if self.rettype then
		return string.format('type function%s: %s',
			common.defstring(self.arglist, lp .. self.lpindent),
			common.defstring(self.rettype, lp .. self.lpindent))
	else
		return string.format('type function%s',
			common.defstring(self.arglist, lp .. self.lpindent))
	end
end

common = require('exl.common')
