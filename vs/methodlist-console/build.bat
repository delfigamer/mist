rem %1 "$(SolutionDir)"
rem %2 $(Platform)-$(Configuration)
cd %~1..
call lua ^
	-e "compactffi=false fileprefix=[[build\%2\client-console\methodlist]] structname=[[client_console_methodlist]]" ^
	"bind.lua" ^
	"utils\cbase.hpp" ^
	"utils\configset.hpp" ^
	"utils\databuffer.hpp" ^
	"utils\encoding.hpp" ^
	"utils\refobject.hpp" ^
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
	"client-console\window.hpp" ^
	"client-console\windowinfo.hpp"
