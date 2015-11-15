local modname = ...
local object = require('base.object')
local token = object:module(modname)

function token:init(type, content, spos, epos)
	self.type = type
	self.content = content
	self.spos = spos
	self.epos = epos
end

function token:gettype()
	return self.type
end

function token:getcontent()
	return self.content
end

function token:getspos()
	return self.spos
end

function token:getepos()
	return self.epos
end

function token:islinestart()
	return self.bislinestart
end

function token.instmeta:__tostring()
	local contentstr
	if type(self.content) == 'string' then
		contentstr = string.format(' %q', self.content)
	elseif type(self.content) == 'nil' then
		contentstr = ''
	else
		contentstr = string.format(' %s', self.content)
	end
	return string.format('%s%s-%s %s%s',
		self.bislinestart and '\n' or '',
		self.spos, self.epos, self.type, contentstr)
end
