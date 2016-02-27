local modname = ...
local ebase = require(modname, 1, 'base')
local escope = ebase:module(modname)
local common

function escope:init(pr)
	ebase.init(self, pr)
	self.base = pr.base
	self.index = pr.index
end

function escope:dobuild(pc)
	self.base:build(pc)
	local proto = {self.base:getfulltype()}
	-- local protostr = {}
	-- for i, arg in ipairs(proto) do
		-- table.append(protostr, arg:defstring(''))
	-- end
	-- protostr = string.format('operator %s(%s)',
		-- self.operator,
		-- table.concat(protostr, ', '))
	local value
	-- do
		-- local itemlist = pc:getop(
			-- self.operator,
			-- proto)
		-- if itemlist then
			-- if #itemlist == 1 then
				-- operatorfunc = itemlist[1].operator
			-- elseif #itemlist > 1 then
				-- local candidates = {}
				-- for i, item in ipairs(itemlist) do
					-- candidates[i] = string.format(
						-- '%s-%s\t%s',
						-- item.operator.spos, item.operator.epos,
						-- item.operator)
				-- end
				-- common.nodeerror(
					-- 'error',
					-- 'ambiguous ' .. protostr .. '\n' ..
						-- 'possible candidates are:\n' ..
						-- table.concat(candidates, '\n'),
					-- self)
			-- end
		-- end
	-- end
	if proto[1].ti.scoperesolve then
		local it = {
			context = pc,
			spos = self.spos,
			epos = self.epos,
			filename = self.filename,
			base = self.base,
			index = self.index,
		}
		self.value = proto[1].ti:scoperesolve(it)
	else
		common.nodeerror(
			string.format('cannot scope values of type %s', proto[1].ti), self)
	end
	if self.value then
		self.constvalue = self.value:getconstvalue()
		self.fulltype = self.value:getfulltype()
	else
		common.nodeerror(
			'cannot resolve scope ' .. tostring(self),
			self)
		return
	end
end

function escope:lcompile(stream, source)
	return self.value:lcompile(stream, source)
end

function escope:rcompile(stream)
	if self.constvalue and self.constvalue.bsimplevalue then
		return self.constvalue:rcompile(stream)
	else
		local name = self.value:rcompile(stream)
		local fts = self.fulltype:getserial()
		return name
	end
end

function escope:defstring(lp)
	return string.format('%s.%s',
		self.base:defstring(lp),
		common.identstring(self.index))
end

common = require(modname, 3, 'common')
