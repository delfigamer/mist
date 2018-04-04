local modname = ...
local env = require('env')
local ffi = require('ffi')

function env.path(str)
	return (string.gsub(str, '/', '\\'))
end

ffi.cdef[[
typedef struct
{
	uint16_t wYear;
	uint16_t wMonth;
	uint16_t wDayOfWeek;
	uint16_t wDay;
	uint16_t wHour;
	uint16_t wMinute;
	uint16_t wSecond;
	uint16_t wMilliseconds;
} systemtime_t;
void __stdcall GetSystemTime(
	systemtime_t* lpSystemTime
);
bool __stdcall SystemTimeToFileTime(
	systemtime_t const* lpSystemTime,
	uint64_t* lpFileTime
);
bool __stdcall CloseHandle(
	void const* hObject
);
void const* __stdcall CreateFileA(
	char const* lpFileName,
	uint32_t dwDesiredAccess,
	uint32_t dwShareMode,
	void const* lpSecurityAttributes,
	uint32_t dwCreationDisposition,
	uint32_t dwFlagsAndAttributes,
	void const* hTemplateFile
);
bool __stdcall GetFileTime(
	void const* hFile,
	uint64_t* lpCreationTime,
	uint64_t* lpLastAccessTime,
	uint64_t* lpLastWriteTime
);
bool __stdcall SetFileTime(
	void const* hFile,
	uint64_t const* lpCreationTime,
	uint64_t const* lpLastAccessTime,
	uint64_t const* lpLastWriteTime
);
]]

function env.getcurrenttime()
	local cst = ffi.new('systemtime_t[1]')
	ffi.C.GetSystemTime(cst)
	local lwt = ffi.new('uint64_t[1]')
	if ffi.C.SystemTimeToFileTime(cst, lwt) then
		return lwt[0]
	else
		error('cannot get current time')
	end
end

function env.getfiletime(path)
	local handle = ffi.C.CreateFileA(
		env.path(path),
		0x80000000, -- GENERIC_READ
		7, -- FILE_SHARE_READ || FILE_SHARE_WRITE || FILE_SHARE_DELETE
		nil,
		3, -- OPEN_EXISTING
		0x02000080, -- FILE_ATTRIBUTE_NORMAL | FILE_FLAG_BACKUP_SEMANTICS
		nil)
	if handle ~= nil then
		local lwt = ffi.new('uint64_t[1]')
		if ffi.C.GetFileTime(handle, nil, nil, lwt) then
			return lwt[0]
		end
		ffi.C.CloseHandle(handle)
	end
end

function env.setfiletime(path, time)
	local handle = ffi.C.CreateFileA(
		env.path(path),
		0x40000000, -- GENERIC_WRITE
		7, -- FILE_SHARE_READ || FILE_SHARE_WRITE || FILE_SHARE_DELETE
		nil,
		3, -- OPEN_EXISTING
		0x80, -- FILE_ATTRIBUTE_NORMAL
		nil)
	if handle ~= nil then
		local lwt = ffi.new('uint64_t[1]')
		if time then
			lwt[0] = time
		else
			local cst = ffi.new('systemtime_t[1]')
			ffi.C.GetSystemTime(cst)
			if not ffi.C.SystemTimeToFileTime(cst, lwt) then
				error('cannot get current time')
			end
		end
		ffi.C.SetFileTime(handle, nil, nil, lwt)
		ffi.C.CloseHandle(handle)
	end
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
