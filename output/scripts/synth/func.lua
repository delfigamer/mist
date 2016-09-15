local modname = ...
local func = package.modtable(modname)

function func.identity(v)
	return v
end

function func.clampfilter(amp)
	return function(v)
		return (math.abs(v+amp) - math.abs(v-amp)) * 0.5
	end
end

func.clamp = func.clampfilter(1)

function func.atanfilter(amp, arg)
	return function(v)
		return math.atan(v*(1/amp))*(amp*2/math.pi)
	end
end

function func.expfilter(amp)
	local function func(v)
		if v < 0 then
			return -func(-v)
		end
		return amp*(1 - math.exp(-v*(1/amp)))
	end
	return func
end

function func.sinwave(phase)
	return math.sin(phase)
end

function func.squarewave(phase)
	return phase < math.pi and 1 or -1
end

function func.trianglewave(phase)
	return math.abs(phase - math.pi) / (math.pi/2) - 1
end
