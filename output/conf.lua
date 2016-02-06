display = {
	gamma = 1 / 2.2,
}
if platform == 'win' then
	display.renderer = 'd3drenderer'
	display.fullscreen = true
elseif platform == 'linux' then
	display.renderer = 'glrenderer'
	display.fullscreen = false
elseif platform == 'android' then
	display.renderer = 'glesrenderer'
	display.fullscreen = true
else
	display.renderer = 'softwarerenderer'
	display.fullscreen = false
end

if display.fullscreen then
	display.width, display.height = 1024, 768
else
	display.width, display.height = 640, 480
end