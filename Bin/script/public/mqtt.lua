--[[
mqtt
--]]

local mqtt = {}
local MsgType = {
    
}

function mqtt.parsePack(pBinary)
    local tInfo = {}
    local tHead = {}
    
    --Õ∑–≈œ¢
    pBinary:setW2R()
    tHead.type = pBinary:getUint8()
    tHead.dup = pBinary:getUint8()
    tHead.qos = pBinary:getUint8()
    tHead.retain = pBinary:getUint8()
    tHead.lens = pBinary:getUint32()    
    tInfo.head = tHead
    pBinary:resetW2R()
    
    
    
    return tInfo
end

