rem %1 "$(SolutionDir)"
rem %2 $(Platform)-$(Configuration)
cd %~1..
call lua ^
	-e "compactffi=false fileprefix=[[build\%2\client-main\methodlist]] structname=[[client_main_methodlist]]" ^
	"bind.lua"<?lua
	for i, unit in ipairs(sources) do
		if unit.type == 'native' then
			if unit.methodlist and find(unit.methodlist, 'client-main') then
				echo(' ^\n\t"', path(unit.name), '.hpp"')
			end
		end
	end
?>
