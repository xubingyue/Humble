--[[
tcp2 
--]]

local tcp2 = {}
local pWBinary = CBinary()

function tcp2.Response(val)
    pWBinary:reSetWrite()    
   
    pWBinary:setUint16(#val)
    pWBinary:setByte(val, #val)
    
    return pWBinary
end

return tcp2
