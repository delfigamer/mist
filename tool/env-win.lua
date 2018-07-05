local modname = ...
local env = require('env')
local ffi = require('ffi')

function env.path(str)
	return (string.gsub(str, '/', '\\'))
end

ffi.cdef[[
	void* __stdcall CreateFileW(
		wchar_t const* lpFileName,
		uint32_t dwDesiredAccess,
		uint32_t dwShareMode,
		void const* lpSecurityAttributes,
		uint32_t dwCreationDisposition,
		uint32_t dwFlagsAndAttributes,
		void* hTemplateFile);
	bool __stdcall CloseHandle(
		void* hObject);
	uint32_t __stdcall GetLastError();
	int __stdcall MultiByteToWideChar(
		unsigned int CodePage, uint32_t dwFlags,
		char const* lpMultiByteStr, int cbMultiByte,
		wchar_t* lpWideCharStr, int cchWideChar);
	int __stdcall WideCharToMultiByte(
		unsigned CodePage, uint32_t dwFlags,
		wchar_t const* lpWideCharStr, int cchWideChar,
		char* lpMultiByteStr, int cbMultiByte,
		char const* lpDefaultChar, bool* lpUsedDefaultChar);
	bool __stdcall GetFileTime(
		void const* hFile,
		uint64_t* lpCreationTime,
		uint64_t* lpLastAccessTime,
		uint64_t* lpLastWriteTime);
]]

local function towstr(ustr)
	local buflen = ffi.C.MultiByteToWideChar(65001, 0, ustr, #ustr, nil, 0)
	local buffer = ffi.new('wchar_t[?]', buflen + 1)
	ffi.C.MultiByteToWideChar(65001, 0, ustr, #ustr, buffer, buflen)
	return buffer, buflen + 1
end

local function fromwstr(wstr, wlen)
	local buflen = ffi.C.WideCharToMultiByte(
		65001, 0, wstr, wlen or -1, nil, 0, nil, nil)
	local buffer = ffi.new('char[?]', buflen)
	ffi.C.WideCharToMultiByte(
		65001, 0, wstr, wlen or -1, buffer, buflen, nil, nil)
	while buflen > 0 and buffer[buflen-1] == 0 do
		buflen = buflen - 1
	end
	return ffi.string(buffer, buflen)
end

local function createfile(path, ...)
	local wpath = towstr(env.path(path))
	local handle = ffi.C.CreateFileW(wpath, ...)
	if ffi.cast('ptrdiff_t', handle) == -1 then
		return nil
	end
	return handle
end

function env.getfiletime(path)
	local handle = createfile(
		path,
		0x80000000, -- GENERIC_READ
		7, -- FILE_SHARE_READ || FILE_SHARE_WRITE || FILE_SHARE_DELETE
		nil,
		3, -- OPEN_EXISTING
		0x02000080, -- FILE_ATTRIBUTE_NORMAL | FILE_FLAG_BACKUP_SEMANTICS
		nil)
	local time
	if handle then
		local lwt = ffi.new('uint64_t[1]')
		if ffi.C.GetFileTime(handle, nil, nil, lwt) then
			time = lwt[0]
		end
		ffi.C.CloseHandle(handle)
	end
	return time
end

function env.rmdir(path)
	-- /s: recursive
	-- /q: silent
	return env.execute('rd \z
		/s \z
		/q \z
		"' .. env.path(path) .. '"')
end

function env.rm(path)
	-- /q: silent
	return env.execute('del \z
		/q \z
		"' .. env.path(path) .. '"')
end

function env.mkdir(path)
	return env.execute('md \z
		"' .. env.path(path) .. '"')
end

function env.copy(source, target)
	-- /b: binary
	-- /y: silent
	return
		env.makepath(target) and
		env.execute('copy \z
			/b \z
			/y \z
			"' .. env.path(source) .. '" \z
			"' .. env.path(target) .. '"')
end
