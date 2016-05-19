local modname = ...
local node = require('mistsh.node')
local funccallexpr = node:module(modname)
local common = require('mistsh.common')

function funccallexpr:init(pr, context)
	node.init(self, pr, context)
	self.func = pr._func
	if pr._args then
		self.args = {}
		for i, apr in ipairs(pr._args) do
			self.args[i] = common.createnode(apr, context)
		end
	end
end

function funccallexpr:defstring(lp)
	if not self.args then
		return self.func .. '()'
	end
	local lines = {}
	for i, arg in ipairs(self.args) do
		lines[i] = arg:defstring(lp .. self.lpindent)
	end
	return self.func .. '( ' .. table.concat(lines, ', ') .. ' )'
end

local funcmap = {}
if common.target == 'hlsl_3_0' then
elseif common.target == 'glsl_100' then
	funcmap.float2 = 'vec2(%*)'
	funcmap.float3 = 'vec3(%*)'
	funcmap.float4 = 'vec4(%*)'
	funcmap.float4x4 = 'mat4(%*)'
	funcmap.tex2D = 'texture2D(%*)'
end

function funccallexpr:build()
	if funcmap[self.func] then
		local function arg(d)
			if d == '*' then
				local lines = {}
				for i, arg in ipairs(self.args) do
					lines[i] = arg:build()
				end
				return table.concat(lines, ', ')
			else
				d = tonumber(d)
				if self.args and self.args[d] then
					return self.args[d]:build()
				else
					error('function mapping failed')
				end
			end
		end
		return string.gsub(funcmap[self.func], '%%([0-9*])', arg)
	end
	local lines = {}
	for i, arg in ipairs(self.args) do
		lines[i] = arg:build()
	end
	return self.func .. '( ' .. table.concat(lines, ', ') .. ' )'
end
