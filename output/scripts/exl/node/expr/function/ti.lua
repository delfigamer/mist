local modname = ...
local baseti = package.relrequire(modname, 2, 'base.ti')
local functionti = baseti:module(modname)
local functioncallof
local common

function functionti:init(pr)
	baseti.init(self, pr)
	self.arglist = pr.arglist
	self.rettype = pr.rettype
	local aser = {}
	for i, arg in ipairs(self.arglist.args) do
		local ti = arg.typev:gettivalue()
		local am = arg.blvalue and (
				arg.brvalue and 'B' or 'L'
			) or (
				arg.brvalue and 'R' or 'N'
			)
		aser[i] = am .. ti:getserial()
	end
	if self.rettype then
		local rti = self.rettype:gettivalue()
		self.serial = string.format(
			'f%i%s%s',
			#aser,
			table.concat(aser),
			rti:getserial())
	else
		self.serial = string.format(
			'f%i%sN',
			#aser,
			table.concat(aser))
	end
end

function functionti:iseq(other)
	if not other['#' .. functionti._NAME] then
		return false
	elseif #self.arglist.args ~= #other.arglist.args then
		return false
	elseif (not self.rettype) ~= (not other.rettype) then
		return false
	elseif self.rettype then
		local srti = self.rettype:gettivalue()
		local orti = other.rettype:gettivalue()
		if not srti or not orti or not srti:iseq(orti) then
			return false
		end
	end
	for i, sarg in ipairs(self.arglist.args) do
		local oarg = other.arglist.args[i]
		if sarg.blvalue ~= oarg.blvalue or sarg.brvalue ~= oarg.brvalue then
			return false
		end
		local sti = sarg.typev:gettivalue()
		local oti = oarg.typev:gettivalue()
		if not sti:iseq(oti) then
			return false
		end
	end
	return true
end

function functionti:internalresolve(op, proto)
	if op == 'call' then
		return functioncallof
	else
		return baseti.internalresolve(self, op, proto)
	end
end

function functionti:defstring(lp)
	if self.rettype then
		return string.format('type function%s: %s',
			self.arglist:defstring(lp .. self.lpindent),
			self.rettype:defstring(lp .. self.lpindent))
	else
		return string.format('type function%s',
			self.arglist:defstring(lp .. self.lpindent))
	end
end

functioncallof = package.relrequire(modname, 3, 'operator.functioncall.factory')
common = package.relrequire(modname, 4, 'common')
