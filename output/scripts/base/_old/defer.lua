local modname = ...
module(modname, package.seeall)
local utility = require('base.utility')

list = {}

function push(...)
	list[#list + 1] = utility.pack(...)
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
