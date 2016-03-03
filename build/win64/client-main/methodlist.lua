local ffi = require('ffi')
ffi.cdef[[typedef bool(*encoder_t)(void*dest,uint32_t charcode,size_t destsize,size_t*pointlength);typedef bool(*decoder_t)(void const*source,uint32_t*charcode,size_t sourcesize,size_t*pointlength);typedef struct{}cbase;typedef struct{}configset;typedef struct{encoder_t encode;decoder_t decode;}encoding_t;typedef struct{encoding_t const*senc;encoding_t const*denc;void const*source;void*dest;size_t sourcesize;size_t destsize;uint32_t defaultchar;size_t sourceresult;size_t destresult;}translation_t;typedef struct{}encoding;typedef struct{}refobject;typedef struct{}rsbin;typedef struct{}fstask;typedef struct{}fileio;typedef struct{}pngreader;typedef struct{}pngwriter;typedef struct{}resource;typedef struct{}shape;typedef struct{int name;int arg[4];}event;typedef struct{}window;typedef struct{}databuffer;typedef struct{bool(*utils_cbase_geterror)(char const**result);bool(*utils_cbase_seterror)(char const*error);bool(*utils_cbase_yield)();bool(*utils_cbase_write)(char const*str);bool(*utils_cbase_getchar)(char*str);bool(*utils_configset_integer)(configset const*self,char const*expr,int def,int*result);bool(*utils_configset_number)(configset const*self,char const*expr,double def,double*result);bool(*utils_configset_lstring)(configset const*self,char const*expr,char*buffer,int buflen,int*result);bool(*utils_configset_boolean)(configset const*self,char const*expr,bool def,bool*result);bool(*utils_databuffer_pcreate)(int length,int capacity,void const*data,databuffer**result);bool(*utils_databuffer_getdata)(databuffer*self,void**result);bool(*utils_databuffer_getlength)(databuffer*self,int*result);bool(*utils_databuffer_setlength)(databuffer*self,int length);bool(*utils_databuffer_getcapacity)(databuffer*self,int*result);bool(*utils_encoding_getencoding)(int index,encoding_t const**result);bool(*utils_encoding_translatestr)(translation_t*translation,int*result);bool(*utils_refobject_addref)(refobject const*self);bool(*utils_refobject_release)(refobject const*self);bool(*rsbin_rsbin_isfileavailable)(char const*path,bool*result);bool(*rsbin_fstask_promote)(fstask*self);bool(*rsbin_fstask_isfinished)(fstask*self,bool*result);bool(*rsbin_fstask_getresult)(fstask*self,int*result);bool(*rsbin_fstask_geterror)(fstask*self,char const**result);bool(*rsbin_fileio_create)(char const*path,int mode,fileio**result);bool(*rsbin_fileio_startread)(fileio*self,uint64_t offset,int length,void*buffer,fstask**result);bool(*rsbin_fileio_startwrite)(fileio*self,uint64_t offset,int length,void const*buffer,fstask**result);bool(*rsbin_fileio_setsize)(fileio*self,uint64_t size);bool(*rsbin_pngreader_create)(int format,pngreader**result);bool(*rsbin_pngreader_release)(pngreader*self);bool(*rsbin_pngreader_feed)(pngreader*self,int length,void const*buffer);bool(*rsbin_pngreader_isfinished)(pngreader*self,bool*result);bool(*rsbin_pngreader_getwidth)(pngreader*self,int*result);bool(*rsbin_pngreader_getheight)(pngreader*self,int*result);bool(*rsbin_pngreader_getdata)(pngreader*self,databuffer**result);bool(*rsbin_pngwriter_create)(int format,int width,int height,databuffer*data,pngwriter**result);bool(*rsbin_pngwriter_release)(pngwriter*self);bool(*rsbin_pngwriter_grab)(pngwriter*self,int length,void*buffer,int*result);bool(*rsbin_pngwriter_isfinished)(pngwriter*self,bool*result);bool(*window_window_setshape)(window*self,shape*nv);bool(*window_window_popevent)(window*self,event*event,bool*result);}methodlist_t;struct WindowInfo{int width;int height;float texelsoffset;float texeltoffset;configset const*configset;bool acceleratorinput;bool pointinput;bool keyboardinput;bool silent;methodlist_t const*methodlist;window*window;};typedef struct WindowInfo windowinfo_t;]]

local windowinfo = package.loaded['host.info']
windowinfo = ffi.cast('windowinfo_t const*', windowinfo)
package.loaded['host.info'] = windowinfo
local methodlist = windowinfo.methodlist
local default_base = require('base.ffipure')
local cbase = default_base:module('host.cbase')
local configset = default_base:module('host.configset')
local encoding_t = default_base:module('host.encoding_t')
local translation_t = default_base:module('host.translation_t')
local encoding = default_base:module('host.encoding')
local refobject = default_base:module('host.refobject')
local rsbin = default_base:module('host.rsbin')
local fstask = refobject:module('host.fstask')
local fileio = refobject:module('host.fileio')
local pngreader = default_base:module('host.pngreader')
local pngwriter = default_base:module('host.pngwriter')
local resource = refobject:module('host.resource')
local shape = resource:module('host.shape')
local event = default_base:module('host.event')
local window = default_base:module('host.window')
local databuffer = refobject:module('host.databuffer')

local function reference_add(self)
	return methodlist.utils_refobject_addref(self)
end

function cbase:geterror(result)
	local result = ffi.new('char const*[1]')
	methodlist.utils_cbase_geterror(result)
		return result[0] ~= nil and result[0]
end

function cbase:seterror(error)
	methodlist.utils_cbase_seterror(error)
		return
end

function cbase:yield()
	methodlist.utils_cbase_yield()
		return
end

function cbase:write(str)
	if methodlist.utils_cbase_write(str) then
		return
	else
		error(cbase:geterror())
	end
end

function cbase:getchar(str)
	if methodlist.utils_cbase_getchar(str) then
		return
	else
		error(cbase:geterror())
	end
end

function configset:integer(expr, def, result)
	local result = ffi.new('int[1]')
	if methodlist.utils_configset_integer(self, expr, def, result) then
		return result[0]
	else
		error(cbase:geterror())
	end
end

function configset:number(expr, def, result)
	local result = ffi.new('double[1]')
	if methodlist.utils_configset_number(self, expr, def, result) then
		return result[0]
	else
		error(cbase:geterror())
	end
end

function configset:lstring(expr, buffer, buflen, result)
	local result = ffi.new('int[1]')
	if methodlist.utils_configset_lstring(self, expr, buffer, buflen, result) then
		return result[0]
	else
		error(cbase:geterror())
	end
end

function configset:boolean(expr, def, result)
	local result = ffi.new('bool[1]')
	if methodlist.utils_configset_boolean(self, expr, def, result) then
		return result[0]
	else
		error(cbase:geterror())
	end
end

function databuffer:pcreate(length, capacity, data, result)
	local result = ffi.new('databuffer*[1]')
	if methodlist.utils_databuffer_pcreate(length, capacity, data, result) then
		return result[0]
	else
		error(cbase:geterror())
	end
end

function databuffer:getdata(result)
	local result = ffi.new('void*[1]')
	methodlist.utils_databuffer_getdata(self, result)
		return result[0]
end

function databuffer:getlength(result)
	local result = ffi.new('int[1]')
	methodlist.utils_databuffer_getlength(self, result)
		return result[0]
end

function databuffer:setlength(length)
	methodlist.utils_databuffer_setlength(self, length)
		return
end

function databuffer:getcapacity(result)
	local result = ffi.new('int[1]')
	methodlist.utils_databuffer_getcapacity(self, result)
		return result[0]
end

function encoding:getencoding(index, result)
	local result = ffi.new('encoding_t const*[1]')
	if methodlist.utils_encoding_getencoding(index, result) then
		return result[0]
	else
		error(cbase:geterror())
	end
end

function encoding:translatestr(translation, result)
	local result = ffi.new('int[1]')
	if methodlist.utils_encoding_translatestr(translation, result) then
		return result[0]
	else
		error(cbase:geterror())
	end
end

function refobject:release()
	methodlist.utils_refobject_release(self)
		self[0] = nil
		return
end

function rsbin:isfileavailable(path, result)
	local result = ffi.new('bool[1]')
	if methodlist.rsbin_rsbin_isfileavailable(path, result) then
		return result[0]
	else
		error(cbase:geterror())
	end
end

function fstask:promote()
	if methodlist.rsbin_fstask_promote(self) then
		return
	else
		error(cbase:geterror())
	end
end

function fstask:isfinished(result)
	local result = ffi.new('bool[1]')
	methodlist.rsbin_fstask_isfinished(self, result)
		return result[0]
end

function fstask:getresult(result)
	local result = ffi.new('int[1]')
	methodlist.rsbin_fstask_getresult(self, result)
		return result[0]
end

function fstask:geterror(result)
	local result = ffi.new('char const*[1]')
	methodlist.rsbin_fstask_geterror(self, result)
		return result[0] ~= nil and result[0]
end

function fileio:create(path, mode, result)
	local result = ffi.new('fileio*[1]')
	if methodlist.rsbin_fileio_create(path, mode, result) then
		return result[0]
	else
		error(cbase:geterror())
	end
end

function fileio:startread(offset, length, buffer, result)
	local result = ffi.new('fstask*[1]')
	if methodlist.rsbin_fileio_startread(self, offset, length, buffer, result) then
		return result[0]
	else
		error(cbase:geterror())
	end
end

function fileio:startwrite(offset, length, buffer, result)
	local result = ffi.new('fstask*[1]')
	if methodlist.rsbin_fileio_startwrite(self, offset, length, buffer, result) then
		return result[0]
	else
		error(cbase:geterror())
	end
end

function fileio:setsize(size)
	if methodlist.rsbin_fileio_setsize(self, size) then
		return
	else
		error(cbase:geterror())
	end
end

function pngreader:create(format, result)
	local result = ffi.new('pngreader*[1]')
	if methodlist.rsbin_pngreader_create(format, result) then
		return result[0]
	else
		error(cbase:geterror())
	end
end

function pngreader:release()
	if methodlist.rsbin_pngreader_release(self) then
		self[0] = nil
		return
	else
		error(cbase:geterror())
	end
end

function pngreader:feed(length, buffer)
	if methodlist.rsbin_pngreader_feed(self, length, buffer) then
		return
	else
		error(cbase:geterror())
	end
end

function pngreader:isfinished(result)
	local result = ffi.new('bool[1]')
	methodlist.rsbin_pngreader_isfinished(self, result)
		return result[0]
end

function pngreader:getwidth(result)
	local result = ffi.new('int[1]')
	methodlist.rsbin_pngreader_getwidth(self, result)
		return result[0]
end

function pngreader:getheight(result)
	local result = ffi.new('int[1]')
	methodlist.rsbin_pngreader_getheight(self, result)
		return result[0]
end

function pngreader:getdata(result)
	local result = ffi.new('databuffer*[1]')
	methodlist.rsbin_pngreader_getdata(self, result)
		reference_addref(result[0])
		return result[0]
end

function pngwriter:create(format, width, height, data, result)
	local result = ffi.new('pngwriter*[1]')
	if methodlist.rsbin_pngwriter_create(format, width, height, data, result) then
		return result[0]
	else
		error(cbase:geterror())
	end
end

function pngwriter:release()
	if methodlist.rsbin_pngwriter_release(self) then
		self[0] = nil
		return
	else
		error(cbase:geterror())
	end
end

function pngwriter:grab(length, buffer, result)
	if methodlist.rsbin_pngwriter_grab(self, length, buffer, result) then
		return
	else
		error(cbase:geterror())
	end
end

function pngwriter:isfinished(result)
	local result = ffi.new('bool[1]')
	if methodlist.rsbin_pngwriter_isfinished(self, result) then
		return result[0]
	else
		error(cbase:geterror())
	end
end

function window:setshape(nv)
	if methodlist.window_window_setshape(self, nv) then
		return
	else
		error(cbase:geterror())
	end
end

function window:popevent(event, result)
	local result = ffi.new('bool[1]')
	if methodlist.window_window_popevent(self, event, result) then
		return result[0]
	else
		error(cbase:geterror())
	end
end

function configset:string(expr, def)
	local buflen = self:lstring(expr, nil, 0)
	if buflen == 0 then
		return def
	end
	local buffer = ffi.new('char[?]', buflen)
	self:lstring(expr, buffer, buflen)
	return ffi.string(buffer, buflen)
end
cbase.typedef = 'cbase'
cbase:buildmetatype()
configset.typedef = 'configset'
configset:buildmetatype()
encoding_t.typedef = 'encoding_t'
encoding_t:buildmetatype()
translation_t.typedef = 'translation_t'
translation_t:buildmetatype()
encoding.typedef = 'encoding'
encoding:buildmetatype()
refobject.typedef = 'refobject'
refobject:buildmetatype()
rsbin.typedef = 'rsbin'
rsbin:buildmetatype()
fstask.typedef = 'fstask'
fstask:buildmetatype()
fileio.typedef = 'fileio'
fileio:buildmetatype()
pngreader.typedef = 'pngreader'
pngreader:buildmetatype()
pngwriter.typedef = 'pngwriter'
pngwriter:buildmetatype()
resource.typedef = 'resource'
resource:buildmetatype()
shape.typedef = 'shape'
shape:buildmetatype()
event.typedef = 'event'
event:buildmetatype()
window.typedef = 'window'
window:buildmetatype()
databuffer.typedef = 'databuffer'
databuffer:buildmetatype()

package.loaded['host.window'] = windowinfo.window
