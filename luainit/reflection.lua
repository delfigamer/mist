local ffipure = require('base.ffipure')
local ffi = require('ffi')

local suppresstypechecks = false

ffi.cdef([[
	typedef struct reflection_exceptionbox {
		void(*freeptr)(struct reflection_exceptionbox* self);
		char const*(*whatptr)(struct reflection_exceptionbox* self);
		void* data[4];
	} reflection_exceptionbox;
]])

local exceptionbox = ffipure:module('reflection.exceptionbox')
exceptionbox.typedef = "reflection_exceptionbox"

function exceptionbox:release()
	if self.freeptr ~= nil then
		self.freeptr(self)
		self.freeptr = nil
		self.whatptr = nil
	end
end

function exceptionbox:what()
	if self.whatptr then
		return ffi.string(self.whatptr(self))
	else
		return ''
	end
end

exceptionbox:buildmetatype()

local objectbox = ffipure:module('reflection.objectbox')

function objectbox:release()
	if self.object ~= nil and self.cfreeptr ~= nil then
		self.cfreeptr(self)
		self.object = nil
		self.cfreeptr = nil
	end
end

package.loaded['reflection.wrapobject'] =
function(box)
	if box.object ~= nil then
		return ffi.gc(box, box.release)
	else
		return nil
	end
end

package.loaded['reflection.unwrapobject'] =
function(value, tag)
	if value then
		if type(value) ~= 'cdata' or not value[tag] then
			error('object of type ' .. tag .. ' expected')
		end
		return value.object
	else
		return nil
	end
end
