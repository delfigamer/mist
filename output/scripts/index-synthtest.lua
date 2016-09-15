local synth = require('synth')

synth.export{
	synth.modulator:create{
		source = synth.generator:create{
			amp = 0,
			freq = 120,
			freqscale = 0.5,
		},
		mode = 'phase',
		target = synth.generator:create{
			freq = 120,
			amp = 0.5,
			wave = synth.func.sinewave,
		},
	},
	path = 'test.flac',
	samples = synth.common.samplerate*5,
	lowbit = true,
}
