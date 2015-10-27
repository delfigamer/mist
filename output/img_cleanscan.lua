function main()
	local mainconf = require('main')
	require('img')
	local function gauss_gen(dev)
		local ef = -0.5/(dev*dev)
		local vf = 1/(math.sqrt(2*math.pi)*dev)
		return function(x)
			return vf * math.exp(x*x*ef)
		end
	end
	local radius = mainconf.radius or 5
	local gaussf = gauss_gen(radius)
	local gauss_x = generate{
		width = radius*2+1,
		height = 1,
		planes = 1,
		function(x, y)
			return gaussf(x - radius)
		end,
	}
	local gauss_y = generate{
		width = 1,
		height = radius*2+1,
		planes = 1,
		function(x, y)
			return gaussf(y - radius)
		end,
	}
	local source = import{
		mainconf.input or 'input.png',
		gray = true,
	}
	local lmax = locallims{source, radius = radius}
	lmax = convolute{lmax, gauss_x}
	lmax = convolute{lmax, gauss_y}
	export{'o1.png', lmax}
	-- local norm = divide{source, lmax}
	-- norm:save(mainconf.output or 'output.fmat')
end

print(pcall(main)); exit();
