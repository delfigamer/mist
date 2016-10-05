rem %1 "$(SolutionDir)"
rem %2 $(Platform)-$(Configuration)
cd %~1..
call lua ^
	-e "compactffi=false fileprefix=[[build\%2\client-main\methodlist]] structname=[[client_main_methodlist]]" ^
	"bind.lua" ^
	"common\databuffer.hpp" ^
	"common\refobject.hpp" ^
	"utils\cbase.hpp" ^
	"utils\configset.hpp" ^
	"utils\encoding.hpp" ^
	"utils\path.hpp" ^
	"rsbin\common.hpp" ^
	"rsbin\fileio.hpp" ^
	"rsbin\flaccommon.hpp" ^
	"rsbin\flacreader.hpp" ^
	"rsbin\flacwriter.hpp" ^
	"rsbin\io.hpp" ^
	"rsbin\iotask.hpp" ^
	"rsbin\memoryio.hpp" ^
	"rsbin\pngcommon.hpp" ^
	"rsbin\pngreader.hpp" ^
	"rsbin\pngwriter.hpp" ^
	"client-main\event.hpp" ^
	"client-main\window.hpp" ^
	"client-main\windowinfo.hpp"
