rem %1 "$(SolutionDir)"
rem %2 $(Platform)-$(Configuration)
cd %~1..
call lua ^
	-e "compactffi=false fileprefix=[[build\%2\renderer-gles\methodlist]] structname=[[renderer_gles_methodlist]]" ^
	"bind.lua" ^
	"renderer-state\programtranslator.hpp" ^
	"renderer-state\resource.hpp" ^
	"renderer-state\shape.hpp" ^
	"renderer-state\shapegroup.hpp" ^
	"renderer-gles\clearshape.hpp" ^
	"renderer-gles\common.hpp" ^
	"renderer-gles\primitiveshape.hpp" ^
	"renderer-gles\staticvertexbuffer.hpp" ^
	"renderer-gles\vertexbuffer.hpp" ^
	"renderer-gles\vertexdeclaration.hpp"
