local modname = ...
local ebase = package.relrequire(modname, 2, 'base')
local efunctionbase = ebase:module(modname)
local bcblock
local common
local context
local fulltype
local functionti

function efunctionbase:init(pr)
	ebase.init(self, pr)
	self.arglist = pr.arglist
	self.body = pr.body
	self.rettype = pr.rettype
	if self.rettype then
		self.resultarg = common.createnode{
			name = 'expr.function.arg',
			spos = self.arglist.epos,
			epos = self.arglist.epos,
			filename = self.arglist.filename,
			lvalue = true,
			rvalue = false,
			typev = pr.rettype,
			target = 'result',
		}
	end
end

function efunctionbase:build(pc)
	self.context = context:create(pc)
	self.arglist:build(self.context)
	if self.resultarg then
		self.resultarg:build(self.context)
	end
	local ti = functionti:create{
		arglist = self.arglist,
		rettype = self.rettype,
	}
	self.fulltype = fulltype:create(ti, false, true)
	self.constvalue = self
end

function efunctionbase:rcompile(stream)
	if not self.retname then
		self.retname = stream:genname()
		local substream = bcblock:create()
		local inargs = self.arglist:getinargs()
		for i, id in ipairs(inargs) do
			inargs[i] = {'local', id}
		end
		local arglocals = self.arglist:getarglocals()
		for i, id in ipairs(arglocals) do
			substream:writetoken{
				op = 'local_create',
				args = {
					{'ssa', 0}, -- value
					{'local', id}, -- id
				},
			}
		end
		if self.resultarg then
			substream:writetoken{
				op = 'local_create',
				args = {
					{'ssa', 0}, -- value
					{'local', self.resultarg.symbol.id}, -- id
				},
			}
		end
		self.body:compile(substream)
		substream:writetoken{
			op = 'ancillary',
			args = {
				{'string', 'comment'}, -- name
				{'int', self.body.epos.row}, -- row
				{'int', self.body.epos.col}, -- col
				{'string', self.body.filename or '-'}, -- filename
				{'string', ''}, -- text
			},
		}
		local outargs = self.arglist:getoutargs()
		local rettokenargs = {}
		if self.resultarg then
			rettokenargs[1] = {'local', self.resultarg.symbol.id}
		end
		for i, id in ipairs(outargs) do
			table.append(rettokenargs, {'local', id})
		end
		substream:writetoken{
			op = 'return',
			args = {
				{'list', items = rettokenargs}, -- values
			},
		}
		stream:writetoken{
			op = 'function',
			args = {
				{'list', items = inargs}, -- args
				{'block', substream}, -- body
				{'ssa', self.retname}, -- target
			},
		}
	end
	return self.retname
end

bcblock = package.relrequire(modname, 4, 'bytecode.block')
common = package.relrequire(modname, 4, 'common')
context = package.relrequire(modname, 4, 'context')
fulltype = package.relrequire(modname, 4, 'fulltype')
functionti = package.relrequire(modname, 1, 'ti')
