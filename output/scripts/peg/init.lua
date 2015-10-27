local modname = ...
module(modname, package.seeall)

local mp = package.modulepack{
	target = _M,
	prefix = modname .. '.',
	names = {
		'andpred',
		'binary',
		'binaryresult',
		'choice',
		'constant',
		'cycleresult',
		'defbuilder',
		'defparser',
		'failure',
		'flatseq',
		'frame',
		'oneormore',
		'optional',
		'notpred',
		'prioritychoice',
		'proxy',
		'rule',
		'sequence',
		'stream',
		'structure',
		'structureresult',
		'terminal',
		'zeroormore'}}

local stream = stream:create(assert(
	io.open(_PATH .. 'scripts/peg/def.peg', 'r')):read('*a'))
local er, ec, def = defparser.file:invoke(stream)
deffile, namespace = defbuilder.build(def)

function loadstring(str, parent)
	local stream = stream:create(str)
	local er, ec, def = deffile:invoke(stream)
	return defbuilder.build(def, parent)
end

function loadfile(path, parent)
	local str = assert(
		io.open(path, 'r'),
		string.format('cannot open %s', path)):read('*a')
	return loadstring(str, parent)
end
