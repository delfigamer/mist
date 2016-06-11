rem %1 "$(SolutionDir)"
rem %2 $(Platform)-$(Configuration)
cd %~1..
call lua ^
	-e "compactffi=false fileprefix=[[build\%2\renderer-gles\methodlist]] structname=[[renderer_gles_methodlist]]" ^
	"bind.lua"<?lua
	for i, unit in ipairs(sources) do
		if unit.type == 'native' then
			if unit.methodlist and find(unit.methodlist, 'renderer-gles') then
				echo(' ^\n\t"', path(unit.name), '.hpp"')
			end
		end
	end
?>
