rem %1 "$(SolutionDir)"
rem %2 $(Platform)-$(Configuration)
cd %~1..
call lua ^
	-e "compactffi=false fileprefix=[[build\%2\client-main\methodlist]]" ^
	"bind.lua" ^
	"utils\cbase.hpp" ^
	"utils\configset.hpp" ^
	"utils\databuffer.hpp" ^
	"utils\encoding.hpp" ^
	"utils\refobject.hpp" ^
	"rsbin\common.hpp" ^
	"rsbin\fileio.hpp" ^
	"rsbin\fsthread.hpp" ^
	"rsbin\pngcommon.hpp" ^
	"rsbin\pngreader.hpp" ^
	"rsbin\pngwriter.hpp" ^
	"graphics\clearshape.hpp" ^
	"graphics\primitiveshape.hpp" ^
	"graphics\resource.hpp" ^
	"graphics\shape.hpp" ^
	"graphics\shapegroup.hpp" ^
	"graphics\staticvertexbuffer.hpp" ^
	"graphics\vertexbuffer.hpp" ^
	"graphics\vertexdeclaration.hpp" ^
	"client-main\event.hpp" ^
	"client-main\window.hpp"
