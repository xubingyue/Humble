--[[
websock
--]]

--[[
enum  WebSockOpCode
{
    WSOCK_CONTINUATION = 0x00,
    WSOCK_TEXTFRAME = 0x01,
    WSOCK_BINARYFRAME = 0x02,
    WSOCK_CLOSE = 0x08,
    WSOCK_PING = 0x09,
    WSOCK_PONG = 0x0A
};--]]

local websock = {}
local pWBinary = CBinary()

function websock.parsePack(pBinary)
    local tInfo = {}
    
    tInfo.info = pBinary:getByte(pBinary:getSurpLens())
    pBinary:setW2R()
    tInfo.code = pBinary:getUint8()
    tInfo.fin = pBinary:getUint8()
    
    return tInfo
end

function websock.Response(code, fin, val)
    pWBinary:reSetWrite()
    
    pWBinary:setUint8(code)
    pWBinary:setUint8(fin)
    if val then
        pWBinary:setByte(val, #val)
    end
    
    return pWBinary
end

--[[分片帧
起始帧（FIN为0，opcode非0），
中间帧（FIN为0，opcode为0），
结束帧（FIN为1，opcode为0）--]]
function websock.ContBegin(code, val)
    assert(1 == code or 2 == code)
    return websock.Response(code, 0, val)
end

function websock.Cont(val)    
    return websock.Response(0, 0, val)
end

function websock.ContEnd(val)
    return websock.Response(0, 1, val)
end

function websock.Text(val)
    return websock.Response(1, 1, val)
end

function websock.Binary(val)
    return websock.Response(2, 1, val)
end

function websock.Close()
    return websock.Response(8, 1)
end

function websock.Ping()
    return websock.Response(9, 1)
end

function websock.Pong()
    return websock.Response(10, 1)
end

return websock
