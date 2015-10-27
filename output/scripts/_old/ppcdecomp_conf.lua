ppcdecomp_config = {
	sourcepath = 'eboot.elf',
	targetpath = 'eboot_decomp.txt',
	offset = -0x10000,
	startia = 0x9cda0,
	subroutine_name = {
		[0x9cda0] = '.program_start',
		[0x30774c] = '.malloc'
	}
}