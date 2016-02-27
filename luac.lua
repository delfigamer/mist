local target, source = ...

target = target or error('target expected')
source = source or error('source expected')

local chunk = assert(loadfile(source))
local f = assert(io.open(target, 'wb'))
f:write(string.dump(chunk, nodebug))
f:close()
