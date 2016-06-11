rem %1 "$(SolutionDir)"
rem %2 $(Platform)-$(Configuration)
cd %~1..
call lua ^
	-e "compactffi=false fileprefix=[[build\%2\renderer-d3d9\methodlist]] structname=[[renderer_d3d9_methodlist]]" ^
	"bind.lua" ^
	"renderer-state\resource.hpp" ^
	"renderer-state\shape.hpp" ^
	"renderer-state\shapegroup.hpp" ^
	"renderer-d3d9\clearshape.hpp" ^
	"renderer-d3d9\common.hpp" ^
	"renderer-d3d9\primitiveshape.hpp" ^
	"renderer-d3d9\programtranslator.hpp" ^
	"renderer-d3d9\staticvertexbuffer.hpp" ^
	"renderer-d3d9\vertexbuffer.hpp" ^
	"renderer-d3d9\vertexdeclaration.hpp"
