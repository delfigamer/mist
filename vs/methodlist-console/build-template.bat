rem %1 "$(SolutionDir)"
rem %2 $(Platform)-$(Configuration)
cd %~1..
call lua ^
	-e "compactffi=false fileprefix=[[build\%2\client-console\methodlist]] structname=[[client_console_methodlist]]" ^
	"bind.lua"<?lua
	for i, unit in ipairs(sources) do
		if unit.type == 'native' then
			if unit.methodlist and find(unit.methodlist, 'client-console') then
				echo(' ^\n\t"', path(unit.name), '.hpp"')
			end
		end
	end
?>
