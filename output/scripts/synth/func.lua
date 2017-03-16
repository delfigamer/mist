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

function func.timefactor(basef)
	return 1/(2*math.pi*basef)
end

function func.inertialnode(t, x)
	assert(t)
	local x = x or 0
	local aw = 1 - math.exp(-common.period/t)
	return function(u)
		x = x + (u - x)*aw
		return x
	end
end

function func.invinertialnode(t, x)
	assert(t)
	local x = x or 0
	local aw = 1 - math.exp(-common.period/t)
	return function(u)
		local d = u - x
		x = x + d*aw
		return d
	end
end

function func.lowpassfilter(cutoff)
	return func.inertialnode(func.timefactor(cutoff))
end

function func.highpassfilter(cutoff)
	return func.invinertialnode(func.timefactor(cutoff))
end

function func.resonantfilter(res, xi)
	assert(res)
	assert(xi)
	local x1 = 0
	local x2 = 0
	local t = func.timefactor(res)
	local k = 0.5*t/xi
	local l = 2*t*xi
	local aw1 = 1 - math.exp(-common.period/k)
	local aw2 = common.period/l
	return function(u)
		x1 = x1 + aw1*(u - x1 - x2)
		x2 = x1*aw2 + x2
		return x1
	end
end
