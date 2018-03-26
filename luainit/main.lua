local ffi = require('ffi')

-- local function loadinitlist(listptr)
	-- local list = ffi.cast(
		-- 'struct{\z
			-- char const* name;\z
			-- char const* data;\z
			-- int length;\z
		-- }*', listptr)
	-- while true do
		-- if list[0].data == nil then
			-- break
		-- end
		-- local name = ffi.string(list[0].name)
		-- local length = list[0].length
		-- local source = ffi.string(list[0].data, length)
		-- local chunk = assert(load(source, name))
		-- assert(pcall(chunk))
		-- list = list + 1
	-- end
-- end

-- if package.loaded['host.info'].renderer_module ~= nil then
	-- loadinitlist(package.loaded['host.info'].renderer_module)
-- end

setfenv(1, _G)
protectglobaltable(true)

local configset = require('utils.mainconf')
local indexmodule = configset:string('index')
if not indexmodule then
	error('index module required')
end

local isuc, err = pcall(require, indexmodule)
if not isuc then
	print(err)
end
