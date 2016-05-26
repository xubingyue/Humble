--[[
default 
--]]

local default = {}
local pWBinary = CBinary()

function default.Response(val)
    pWBinary:reSetWrite()
    
    if val then
        pWBinary:setByte(val, #val)
    end
    
    return pWBinary
end

return default
