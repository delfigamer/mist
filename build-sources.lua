local modname = ...
-- use:
--   client-main
--   renderer-d3d9
--   renderer-gles
local sources = {
	{
		type = 'native',
		name = 'common',
		headeronly = true,
		use = 'client-main;renderer-d3d9;renderer-gles',
	},
	{
		type = 'native',
		name = 'osapi',
		headeronly = true,
		use = 'client-main;renderer-d3d9;renderer-gles',
	},
--------------------------------------------------------------------------------
	{
		type = 'native',
		name = 'common/databuffer',
		headeronly = true,
		use = 'client-main;renderer-d3d9;renderer-gles',
		methodlist = 'client-main',
	},
	{
		type = 'native',
		name = 'common/flaglock',
		headeronly = true,
		use = 'client-main;renderer-d3d9;renderer-gles',
	},
	{
		type = 'native',
		name = 'common/mpsclist',
		headeronly = true,
		use = 'client-main;renderer-d3d9;renderer-gles',
	},
	{
		type = 'native',
		name = 'common/mpscqueue',
		headeronly = true,
		use = 'client-main;renderer-d3d9;renderer-gles',
	},
	{
		type = 'native',
		name = 'common/ref',
		headeronly = true,
		use = 'client-main;renderer-d3d9;renderer-gles',
	},
	{
		type = 'native',
		name = 'common/refobject',
		headeronly = true,
		use = 'client-main;renderer-d3d9;renderer-gles',
		methodlist = 'client-main',
	},
	{
		type = 'native',
		name = 'common/strexception',
		headeronly = true,
		use = 'client-main;renderer-d3d9;renderer-gles',
	},
	{
		type = 'native',
		name = 'common/string',
		headeronly = true,
		use = 'client-main;renderer-d3d9;renderer-gles',
	},
	{
		type = 'native',
		name = 'common/stringbuilder',
		headeronly = true,
		use = 'client-main;renderer-d3d9;renderer-gles',
	},
--------------------------------------------------------------------------------
	{
		type = 'native',
		name = 'utils/cbase',
		use = 'client-main',
		methodlist = 'client-main',
	},
	{
		type = 'native',
		name = 'utils/configset',
		use = 'client-main',
		methodlist = 'client-main',
	},
	{
		type = 'native',
		name = 'utils/console',
		use = 'client-main',
	},
	{
		type = 'native',
		name = 'utils/cyclicbuffer',
		use = 'client-main',
	},
	{
		type = 'native',
		name = 'utils/encoding',
		use = 'client-main',
		methodlist = 'client-main',
	},
	{
		type = 'native',
		name = 'utils/path',
		use = 'client-main',
		methodlist = 'client-main',
	},
	{
		type = 'native',
		name = 'utils/pathparse',
		headeronly = true,
		use = 'client-main',
	},
	{
		type = 'native',
		name = 'utils/sexpr',
		use = 'client-main',
		methodlist = 'client-main',
	},
--------------------------------------------------------------------------------
	{
		type = 'native',
		name = 'exl/construct',
		use = 'client-main',
	},
	{
		type = 'native',
		name = 'exl/context',
		use = 'client-main',
	},
	{
		type = 'native',
		name = 'exl/error',
		use = 'client-main',
	},
	{
		type = 'native',
		name = 'exl/format',
		use = 'client-main',
	},
	{
		type = 'native',
		name = 'exl/func',
		use = 'client-main',
	},
	{
		type = 'native',
		name = 'exl/luainterface',
		use = 'client-main',
		methodlist = 'client-main',
	},
	{
		type = 'native',
		name = 'exl/object',
		use = 'client-main',
	},
	{
		type = 'native',
		name = 'exl/types',
		headeronly = true,
		use = 'client-main',
	},
----------------------------------------
	{
		type = 'native',
		name = 'exl/il/translate',
		use = 'client-main',
		methodlist = 'client-main',
	},
----------------------------------------
	{
		type = 'native',
		name = 'exl/node/argdef',
		use = 'client-main',
	},
	{
		type = 'native',
		name = 'exl/node/argdeflist',
		use = 'client-main',
	},
	{
		type = 'native',
		name = 'exl/node/base',
		use = 'client-main',
	},
	{
		type = 'native',
		name = 'exl/node/block',
		use = 'client-main',
	},
----------------------------------------
	{
		type = 'native',
		name = 'exl/node/expr/base',
		use = 'client-main',
	},
	{
		type = 'native',
		name = 'exl/node/expr/function',
		use = 'client-main',
	},
	{
		type = 'native',
		name = 'exl/node/expr/functiontype',
		use = 'client-main',
	},
	{
		type = 'native',
		name = 'exl/node/expr/invoke',
		use = 'client-main',
	},
	{
		type = 'native',
		name = 'exl/node/expr/name',
		use = 'client-main',
	},
	{
		type = 'native',
		name = 'exl/node/expr/number',
		use = 'client-main',
	},
	{
		type = 'native',
		name = 'exl/node/expr/string',
		use = 'client-main',
	},
	{
		type = 'native',
		name = 'exl/node/expr/subexpression',
		use = 'client-main',
	},
----------------------------------------
	{
		type = 'native',
		name = 'exl/node/file/unit',
		use = 'client-main',
	},
----------------------------------------
	{
		type = 'native',
		name = 'exl/node/stat/conditional',
		use = 'client-main',
	},
	{
		type = 'native',
		name = 'exl/node/stat/const',
		use = 'client-main',
	},
	{
		type = 'native',
		name = 'exl/node/stat/expression',
		use = 'client-main',
	},
	{
		type = 'native',
		name = 'exl/node/stat/function',
		use = 'client-main',
	},
	{
		type = 'native',
		name = 'exl/node/stat/local',
		use = 'client-main',
	},
----------------------------------------
	{
		type = 'native',
		name = 'exl/parser/ast',
		use = 'client-main',
		methodlist = 'client-main',
	},
	{
		type = 'native',
		name = 'exl/parser/charstream',
		use = 'client-main',
	},
	{
		type = 'native',
		name = 'exl/parser/exllexer',
		use = 'client-main',
	},
	{
		type = 'native',
		name = 'exl/parser/exlparser',
		use = 'client-main',
		methodlist = 'client-main',
	},
----------------------------------------
	{
		type = 'native',
		name = 'exl/symbol/base',
		use = 'client-main',
	},
	{
		type = 'native',
		name = 'exl/symbol/const',
		use = 'client-main',
	},
	{
		type = 'native',
		name = 'exl/symbol/local',
		use = 'client-main',
	},
----------------------------------------
	{
		type = 'native',
		name = 'exl/system/context',
		use = 'client-main',
	},
----------------------------------------
	{
		type = 'native',
		name = 'exl/system/value/operator',
		use = 'client-main',
	},
----------------------------------------
	{
		type = 'native',
		name = 'exl/system/typeinfo/number',
		use = 'client-main',
	},
	{
		type = 'native',
		name = 'exl/system/typeinfo/string',
		use = 'client-main',
	},
	{
		type = 'native',
		name = 'exl/system/typeinfo/typedef',
		use = 'client-main',
	},
----------------------------------------
	{
		type = 'native',
		name = 'exl/system/operator/add',
		use = 'client-main',
	},
	{
		type = 'native',
		name = 'exl/system/operator/assign',
		use = 'client-main',
	},
	{
		type = 'native',
		name = 'exl/system/operator/binary',
		use = 'client-main',
	},
	{
		type = 'native',
		name = 'exl/system/operator/join',
		use = 'client-main',
	},
	{
		type = 'native',
		name = 'exl/system/operator/numberbinary',
		use = 'client-main',
	},
	{
		type = 'native',
		name = 'exl/system/operator/subtract',
		use = 'client-main',
	},
----------------------------------------
	{
		type = 'native',
		name = 'exl/typeinfo/base',
		use = 'client-main',
	},
	{
		type = 'native',
		name = 'exl/typeinfo/function',
		use = 'client-main',
	},
----------------------------------------
	{
		type = 'native',
		name = 'exl/value/base',
		use = 'client-main',
	},
	{
		type = 'native',
		name = 'exl/value/function',
		use = 'client-main',
	},
	{
		type = 'native',
		name = 'exl/value/functiontype',
		use = 'client-main',
	},
	{
		type = 'native',
		name = 'exl/value/identitycast',
		use = 'client-main',
	},
	{
		type = 'native',
		name = 'exl/value/literal',
		use = 'client-main',
	},
	{
		type = 'native',
		name = 'exl/value/number',
		use = 'client-main',
	},
	{
		type = 'native',
		name = 'exl/value/proxy',
		use = 'client-main',
	},
	{
		type = 'native',
		name = 'exl/value/reference',
		use = 'client-main',
	},
	{
		type = 'native',
		name = 'exl/value/string',
		use = 'client-main',
	},
	{
		type = 'native',
		name = 'exl/value/typedef',
		use = 'client-main',
	},
--------------------------------------------------------------------------------
	{
		type = 'native',
		name = 'rsbin/common',
		use = 'client-main',
		methodlist = 'client-main',
	},
	{
		type = 'native',
		name = 'rsbin/fileio',
		use = 'client-main',
		methodlist = 'client-main',
	},
	{
		type = 'native',
		name = 'rsbin/flaccommon',
		headeronly = true,
		use = 'client-main',
		methodlist = 'client-main',
	},
	{
		type = 'native',
		name = 'rsbin/flacreader',
		use = 'client-main',
		methodlist = 'client-main',
	},
	{
		type = 'native',
		name = 'rsbin/flacwriter',
		use = 'client-main',
		methodlist = 'client-main',
	},
	{
		type = 'native',
		name = 'rsbin/fsthread',
		use = 'client-main',
	},
	{
		type = 'native',
		name = 'rsbin/io',
		use = 'client-main',
		methodlist = 'client-main',
	},
	{
		type = 'native',
		name = 'rsbin/iotask',
		use = 'client-main',
		methodlist = 'client-main',
	},
	{
		type = 'native',
		name = 'rsbin/memoryio',
		use = 'client-main',
		methodlist = 'client-main',
	},
	{
		type = 'native',
		name = 'rsbin/pngcommon',
		headeronly = true,
		use = 'client-main',
		methodlist = 'client-main',
	},
	{
		type = 'native',
		name = 'rsbin/pngreader',
		use = 'client-main',
		methodlist = 'client-main',
	},
	{
		type = 'native',
		name = 'rsbin/pngwriter',
		use = 'client-main',
		methodlist = 'client-main',
	},
--------------------------------------------------------------------------------
	{
		type = 'native',
		name = 'renderer-state/common',
		headeronly = true,
		use = 'renderer-d3d9;renderer-gles',
	},
	{
		type = 'native',
		name = 'renderer-state/context',
		headeronly = true,
		use = 'renderer-d3d9;renderer-gles',
	},
	{
		type = 'native',
		name = 'renderer-state/context-cpp',
		headeronly = true,
		use = 'renderer-d3d9;renderer-gles',
	},
	{
		type = 'native',
		name = 'renderer-state/interface',
		headeronly = true,
		use = 'renderer-d3d9;renderer-gles',
	},
	{
		type = 'native',
		name = 'renderer-state/interface-cpp',
		headeronly = true,
		use = 'renderer-d3d9;renderer-gles',
	},
	{
		type = 'native',
		name = 'renderer-state/programtranslator',
		headeronly = true,
		use = 'renderer-d3d9;renderer-gles',
		methodlist = 'renderer-d3d9;renderer-gles',
	},
	{
		type = 'native',
		name = 'renderer-state/programtranslator-cpp',
		headeronly = true,
		use = 'renderer-d3d9;renderer-gles',
	},
	{
		type = 'native',
		name = 'renderer-state/programtranslator-base',
		use = 'renderer-d3d9;renderer-gles',
		headeronly = true,
	},
	{
		type = 'native',
		name = 'renderer-state/programtranslator-functions',
		use = 'renderer-d3d9;renderer-gles',
		headeronly = true,
	},
	{
		type = 'native',
		name = 'renderer-state/programtranslator-generators',
		use = 'renderer-d3d9;renderer-gles',
		headeronly = true,
	},
	{
		type = 'native',
		name = 'renderer-state/programtranslator-shaders',
		use = 'renderer-d3d9;renderer-gles',
		headeronly = true,
	},
	{
		type = 'native',
		name = 'renderer-state/programtranslator-state',
		use = 'renderer-d3d9;renderer-gles',
		headeronly = true,
	},
	{
		type = 'native',
		name = 'renderer-state/programtranslator-step',
		use = 'renderer-d3d9;renderer-gles',
		headeronly = true,
	},
	{
		type = 'native',
		name = 'renderer-state/programtranslator-swizzles',
		use = 'renderer-d3d9;renderer-gles',
		headeronly = true,
	},
	{
		type = 'native',
		name = 'renderer-state/resource',
		headeronly = true,
		use = 'renderer-d3d9;renderer-gles',
		methodlist = 'renderer-d3d9;renderer-gles',
	},
	{
		type = 'native',
		name = 'renderer-state/resource-cpp',
		headeronly = true,
		use = 'renderer-d3d9;renderer-gles',
	},
	{
		type = 'native',
		name = 'renderer-state/shape',
		headeronly = true,
		use = 'renderer-d3d9;renderer-gles',
		methodlist = 'renderer-d3d9;renderer-gles',
	},
	{
		type = 'native',
		name = 'renderer-state/shape-cpp',
		headeronly = true,
		use = 'renderer-d3d9;renderer-gles',
	},
	{
		type = 'native',
		name = 'renderer-state/shapegroup',
		headeronly = true,
		use = 'renderer-d3d9;renderer-gles',
		methodlist = 'renderer-d3d9;renderer-gles',
	},
	{
		type = 'native',
		name = 'renderer-state/shapegroup-cpp',
		headeronly = true,
		use = 'renderer-d3d9;renderer-gles',
	},
--------------------------------------------------------------------------------
	{
		type = 'native',
		name = 'renderer-d3d9/clearshape',
		use = 'renderer-d3d9',
		methodlist = 'renderer-d3d9',
	},
	{
		type = 'native',
		name = 'renderer-d3d9/common',
		use = 'renderer-d3d9',
		methodlist = 'renderer-d3d9',
	},
	{
		type = 'native',
		name = 'renderer-d3d9/context',
		use = 'renderer-d3d9',
	},
	{
		type = 'native',
		name = 'renderer-d3d9/display',
		use = 'renderer-d3d9',
	},
	{
		type = 'native',
		name = 'renderer-d3d9/interface',
		use = 'renderer-d3d9',
	},
	{
		type = 'native',
		name = 'renderer-d3d9/primitiveshape',
		use = 'renderer-d3d9',
		methodlist = 'renderer-d3d9',
	},
	{
		type = 'native',
		name = 'renderer-d3d9/program',
		use = 'renderer-d3d9',
		methodlist = 'renderer-d3d9',
	},
	{
		type = 'native',
		name = 'renderer-d3d9/programtranslator',
		use = 'renderer-d3d9',
	},
	{
		type = 'native',
		name = 'renderer-d3d9/resource',
		use = 'renderer-d3d9',
	},
	{
		type = 'native',
		name = 'renderer-d3d9/shape',
		use = 'renderer-d3d9',
	},
	{
		type = 'native',
		name = 'renderer-d3d9/shapegroup',
		use = 'renderer-d3d9',
	},
	{
		type = 'native',
		name = 'renderer-d3d9/staticvertexbuffer',
		use = 'renderer-d3d9',
		methodlist = 'renderer-d3d9',
	},
	{
		type = 'native',
		name = 'renderer-d3d9/vertexbuffer',
		use = 'renderer-d3d9',
		methodlist = 'renderer-d3d9',
	},
	{
		type = 'native',
		name = 'renderer-d3d9/vertexdeclaration',
		use = 'renderer-d3d9',
		methodlist = 'renderer-d3d9',
	},
--------------------------------------------------------------------------------
	{
		type = 'native',
		name = 'renderer-gles/clearshape',
		use = 'renderer-gles',
		methodlist = 'renderer-gles',
	},
	{
		type = 'native',
		name = 'renderer-gles/common',
		use = 'renderer-gles',
		methodlist = 'renderer-gles',
	},
	{
		type = 'native',
		name = 'renderer-gles/context',
		use = 'renderer-gles',
	},
	{
		type = 'native',
		name = 'renderer-gles/display',
		use = 'renderer-gles',
	},
	{
		type = 'native',
		name = 'renderer-gles/gl2',
		use = 'renderer-gles',
		headeronly = true,
	},
	{
		type = 'native',
		name = 'renderer-gles/interface',
		use = 'renderer-gles',
	},
	{
		type = 'native',
		name = 'renderer-gles/primitiveshape',
		use = 'renderer-gles',
		methodlist = 'renderer-gles',
	},
	{
		type = 'native',
		name = 'renderer-gles/program',
		use = 'renderer-gles',
		methodlist = 'renderer-gles',
	},
	{
		type = 'native',
		name = 'renderer-gles/programtranslator',
		use = 'renderer-gles',
	},
	{
		type = 'native',
		name = 'renderer-gles/resource',
		use = 'renderer-gles',
	},
	{
		type = 'native',
		name = 'renderer-gles/shape',
		use = 'renderer-gles',
	},
	{
		type = 'native',
		name = 'renderer-gles/shapegroup',
		use = 'renderer-gles',
	},
	{
		type = 'native',
		name = 'renderer-gles/staticvertexbuffer',
		use = 'renderer-gles',
		methodlist = 'renderer-gles',
	},
	{
		type = 'native',
		name = 'renderer-gles/vertexbuffer',
		use = 'renderer-gles',
		methodlist = 'renderer-gles',
	},
	{
		type = 'native',
		name = 'renderer-gles/vertexdeclaration',
		use = 'renderer-gles',
		methodlist = 'renderer-gles',
	},
--------------------------------------------------------------------------------
	{
		type = 'native',
		name = 'client-main/event',
		headeronly = true,
		use = 'client-main',
		methodlist = 'client-main',
	},
	{
		type = 'native',
		name = 'client-main/main',
		noheader = true,
		use = 'client-main',
	},
	{
		type = 'native',
		name = 'client-main/window',
		use = 'client-main',
		methodlist = 'client-main',
	},
	{
		type = 'native',
		name = 'client-main/windowinfo',
		headeronly = true,
		use = 'client-main;renderer-d3d9;renderer-gles',
		methodlist = 'client-main',
	},
--------------------------------------------------------------------------------
	{
		type = 'luainit',
		name = 'luainit/main',
	},
	{
		type = 'luainit',
		name = 'luainit/baselib',
	},
	{
		type = 'luainit',
		name = 'luainit/object',
	},
	{
		type = 'luainit',
		name = 'luainit/ffipure',
	},
	{
		type = 'luainit',
		name = 'luainit/hostlib',
	},
}

package.modtable(modname, sources)
