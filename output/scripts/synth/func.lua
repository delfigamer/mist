local modname = ...
local func = package.modtable(modname)
local common = require(modname, 1, 'common')

function func.clamp(v)
	return 0.5*(math.abs(v+1) - math.abs(v-1))
end

function func.atannorm(v)
	return (2/math.pi)*math.atan(v)
end

function func.expnorm(v)
	if v < 0 then
		return -func.expfilter(-v)
	end
	return 1 - math.exp(-v)
end

function func.square(phase)
	return phase < math.pi and 1 or -1
end

function func.triangle(phase)
	return math.abs(phase - math.pi) / (math.pi/2) - 1
end

function func.wavegen(basefunc)
	local basefunc = basefunc or math.sin
	local phase = 0
	return function(frequency, dphase)
		dphase = dphase or 0
		local value = basefunc(phase + dphase)
		phase = math.fmod(
			phase + 2*math.pi*common.period*frequency,
			math.pi*2)
		return value
	end
end

function func.lowpassgen(a, x)
	assert(a)
	local x = x or 0
	local aw = a*common.period
	return function(u)
		x = x + (u - x)*aw
		return x
	end
end
