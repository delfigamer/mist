local modname = 'base.defer'
local defer = package.modtable(modname)

defer.list = {}

local function pack(...)
	local argc, argv = select('#', ...), {...}
	return function(a, b)
		return unpack(argv, a or 1, b or argc)
	end
end

function push(...)
	list[#list + 1] = pack(...)
end

function run()
	local i = 1
	local op = list[i]
	while op do
		local suc, result = pcall(op())
		if not suc then
			log('! error occured while running a deferred function:', result)
		end
		if not suc or result then
			local length = #list
			if i < length then
				list[i] = table.pop(list)
			else
				list[i] = nil
				break
			end
		else
			i = i + 1
			op = list[i]
		end
	end
end
