local modname = ...
local baseti = require(modname, 2, 'base.ti')
local functionti = baseti:module(modname)
local common

function functionti:init(pr)
	baseti.init(self, pr)
	self.args = pr.args
	self.rettype = pr.rettype
	local aser = {}
	for i, arg in ipairs(self.args) do
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
	elseif #self.args ~= #other.args then
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
	for i, sarg in ipairs(self.args) do
		local oarg = other.args[i]
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

function functionti:defstring(lp)
	local argstr = {}
	for i, arg in ipairs(self.args) do
		argstr[i] = arg:defstring(lp .. self.lpindent)
	end
	if self.rettype then
		return string.format('type function(%s): %s',
			table.concat(argstr, ', '),
			self.rettype:defstring(lp .. self.lpindent))
	else
		return string.format('type function(%s)',
			table.concat(argstr, ', '))
	end
end

common = require(modname, 4, 'common')
