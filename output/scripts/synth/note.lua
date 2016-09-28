local modname = ...
local note = package.modtable(modname)

note.basefreq = 440 / math.pow(2, 4 + 9/12) -- C0 in A440
note.majorscale = {0, 2, 4, 5, 7, 9, 11}
note.minorscale = {0, 2, 3, 5, 7, 8, 10}
note.channels = {
	A = 1,
	B = 2,
	C = 3,
	D = 4,
	E = 5,
	F = 6,
	G = 7,
	H = 8,
}
note.notenames = {
	c = 1,
	d = 2,
	e = 3,
	f = 4,
	g = 5,
	h = 6,
	a = 6,
	b = 7,
}

function note.freq(octave, semitone, cent)
	return note.basefreq * math.pow(2, octave + semitone/12 + cent/1200)
end

function note.parselength(arg, default)
	local a = string.match(arg, '([0-9]*)%.')
	if a then
		if #a > 0 then
			return 6 / tonumber(a)
		else
			return 6 / default
		end
	else
		if #arg > 0 then
			return 4 / tonumber(arg)
		else
			return 4 / default
		end
	end
end

local function newct(chcount)
	local ct = {}
	for i = 1, chcount do
		ct[i] = {
			octave = 3,
			cent = 0,
			gain = 1,
			lengthscale = 0.5,
			defaultlength = 4,
			totallength = 0,
		}
	end
	return ct
end

local function parse_applycmd(gamut, cti, cmd, arg)
	if note.notenames[cmd] then
		local length = note.parselength(arg, cti.defaultlength)
		local noteindex = note.notenames[cmd]
		local semitone = gamut[noteindex]
		local entry = {
			length * cti.lengthscale,
			freq = note.freq(cti.octave, semitone, cti.cent),
			gain = cti.gain,
		}
		table.append(cti, entry)
		cti.totallength = cti.totallength + entry[1]
	elseif cmd == 'r' or cmd == '_' then
		local length = note.parselength(arg, cti.defaultlength)
		local entry = {
			length * cti.lengthscale,
			freq = 0,
			gain = 0,
		}
		table.append(cti, entry)
		cti.totallength = cti.totallength + entry[1]
	elseif cmd == '-' then
		local length = note.parselength(arg, cti.defaultlength)
		local dt = length * cti.lengthscale
		cti[#cti][1] = cti[#cti][1] + dt
		cti.totallength = cti.totallength + dt
	elseif cmd == 'T' then
		cti.lengthscale = 60 / tonumber(arg)
	elseif cmd == 'V' then
		cti.gain = math.pow(10, tonumber(arg)/20)
	elseif cmd == 'o' then
		cti.octave = tonumber(arg)
	elseif cmd == '>' then
		cti.octave = cti.octave + 1
	elseif cmd == '<' then
		cti.octave = cti.octave - 1
	elseif cmd == 'l' then
		cti.defaultlength = tonumber(arg)
	end
end

function note.parse(it)
	local gamut = it.gamut or note.majorscale
	local chcount = it.chcount or 8
	local ct = newct(chcount)
	local chswitch = true
	local chmap = {}
	for cmd, arg in string.gmatch(it.str, '([a-zA-Z<>_-])([0-9.+-]*)') do
		if note.channels[cmd] then
			if not chswitch then
				chmap = {}
				chswitch = true
			end
			chmap[note.channels[cmd]] = true
		else
			chswitch = false
			for i = 1, chcount do
				if chmap[i] then
					-- print(i, cmd, arg)
					parse_applycmd(gamut, ct[i], cmd, arg)
				end
			end
		end
	end
	local totallength = 0
	for i = 1, chcount do
		if ct[i].totallength > totallength then
			totallength = ct[i].totallength
		end
	end
	return totallength, table.unpack(ct)
end
