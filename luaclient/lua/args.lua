local modname = ...
module(modname, package.seeall)

if not _G.arg then
	return
end

for k, v in pairs(_G.arg) do
	_M[k] = v
end

filename = _M[0]

flags = {}
files = {}
for i, line in ipairs(_M) do
	if line:sub(1,1) == '-' then
		local epos = line:find('=', 1, true)
		if epos then
			local key, value = line:sub(2, epos-1), line:sub(epos+1)
			flags[key] = value
		else
			local key = line:sub(2)
			flags[key] = true
		end
	else
		files[#files + 1] = line
	end
end
