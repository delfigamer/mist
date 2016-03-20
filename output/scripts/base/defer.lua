local modname = ...
local defer = package.modtable(modname)

defer.list = {}

function defer.push(...)
	list[#list + 1] = table.pack(...)
end

function defer.run()
	local i = 1
	local op = defer.list[i]
	while op do
		local suc, result = pcall(table.unpack(op))
		if not suc then
			log('! error occured while running a deferred function:', result)
		end
		if not suc or result then
			local length = #defer.list
			if i < length then
				list[i] = table.pop(list)
			else
				list[i] = nil
				break
			end
		else
			i = i + 1
			op = defer.list[i]
		end
	end
end
