--[[
mqtt
--]]

require("macros")
local table = table
local math = math

local mqtt = {}
local pWBinary = CBinary()

local MsgType = {
    "CONNECT",    --1 客户端请求连接服务器
    "CONNACK",    --2 连接确认
    "PUBLISH",    --3 发布消息
    "PUBACK",     --4 发布确认
    "PUBREC",     --5 发布接收（有保证的交付第1部分）
    "PUBREL",     --6 发布释放（有保证的交付第2部分）
    "PUBCOMP",    --7 发布完成（有保证的交付第3部分）
    "SUBSCRIBE",  --8 客户端订阅请求
    "SUBACK",     --9 订阅确认
    "UNSUBSCRIBE",--10 客户端取消订阅请求
    "UNSUBACK",   --11 取消订阅确认
    "PINGREQ",    --12 PING请求
    "PINGRESP",   --13 PING回复
    "DISCONNECT"  --14 客户端断开连接
}
MsgType = table.enum(MsgType, 1)

--客户端请求连接服务器
local function parseCONNECT(pBinary, tInfo)
    --可变头
    local tVHead = {}
        
    local iLens = pBinary:getUint16()
    tVHead.proname = pBinary:getByte(iLens)
    tVHead.version = pBinary:getUint8()

    local cConnectFlags = pBinary:getUint8()        
    tVHead.userflage = (cConnectFlags & 0x80) >> 7
    tVHead.pswflag = (cConnectFlags & 0x40) >> 6
    tVHead.willretain = (cConnectFlags & 0x20) >> 5
    tVHead.willqos = (cConnectFlags & 0x18) >> 3
    tVHead.will = (cConnectFlags & 0x04) >> 2
    tVHead.cleansession = (cConnectFlags & 0x02) >> 1
    
    tVHead.keepalive = pBinary:getUint16()
        
    tInfo.vhead = tVHead
    
    --有效载荷
    local tPayload = {}
    iLens = pBinary:getUint16()
    tPayload.clientid = pBinary:getByte(iLens)
    if 1 == tVHead.will then
        iLens = pBinary:getUint16()
        tPayload.topic = pBinary:getByte(iLens)
        iLens = pBinary:getUint16()
        tPayload.payload = pBinary:getByte(iLens)
    end    
    if 1 == tVHead.userflage then
        iLens = pBinary:getUint16()
        tPayload.user = pBinary:getByte(iLens)
    end    
    if 1 == tVHead.pswflag then
        iLens = pBinary:getUint16()
        tPayload.psw = pBinary:getByte(iLens)
    end
        
    tInfo.payload = tPayload
end

--连接确认
local function parseCONNACK(pBinary, tInfo)
    --可变头
    local tVHead = {}
    pBinary:skipRead(1)
    tVHead.rtn = pBinary:getUint8()
    
    tInfo.vhead = tVHead
end

local function parsePUBLISH(pBinary, tInfo)
    --可变头
    local tVHead = {}
    local iLens = pBinary:getUint16()
    tVHead.topic = pBinary:getByte(iLens)
    if 1 == tInfo.fhead.qos or 2 == tInfo.fhead.qos then
        tVHead.msgid = pBinary:getUint16()
    end
    
    tInfo.vhead = tVHead
    
    --有效载荷
    local tPayload = {}
    tPayload.info = pBinary:getByte(pBinary:getSurpLens())
    
    tInfo.payload = tPayload
end
local function parsePUBACK(pBinary, tInfo)
end
local function parsePUBREC(pBinary, tInfo)
end
local function parsePUBREL(pBinary, tInfo)
end
local function parsePUBCOMP(pBinary, tInfo)
end
local function parseSUBSCRIBE(pBinary, tInfo)
    --可变头
    local tVHead = {}
    tVHead.msgid = pBinary:getUint16()
    tInfo.vhead = tVHead
    
    --有效载荷
    local tPayload = {}
    while true
    do
        if 2 > pBinary:getSurpLens() then
            break
        end
        
        local iLens = pBinary:getUint16()
        if 0 == iLens then
            break
        end
        
        local tTmp = {}
        tTmp.topic = pBinary:getByte(iLens)
        tTmp.qos = pBinary:getUint8()
        table.insert(tPayload, tTmp)
    end
    
    tInfo.payload = tPayload
end
local function parseSUBACK(pBinary, tInfo)
end
local function parseUNSUBSCRIBE(pBinary, tInfo)
end
local function parseUNSUBACK(pBinary, tInfo)
end
local function parsePINGREQ(pBinary, tInfo)
end
local function parsePINGRESP(pBinary, tInfo)
end
local function parseDISCONNECT(pBinary, tInfo)
end

function mqtt.parsePack(pBinary)
    local tInfo = {}    
    
    --头信息
    local tFixedHead = {}
    pBinary:setW2R()
    local cType = pBinary:getUint8()
    tFixedHead.type = cType
    tFixedHead.dup = pBinary:getUint8()
    tFixedHead.qos = pBinary:getUint8()
    tFixedHead.retain = pBinary:getUint8()
    tFixedHead.lens = pBinary:getUint32()    
    tInfo.fhead = tFixedHead
    pBinary:resetW2R()
    
    if MsgType.CONNECT == cType then    --客户端请求连接服务器
        parseCONNECT(pBinary, tInfo)
    elseif MsgType.CONNACK == cType then  --连接确认
        parseCONNACK(pBinary, tInfo)
    elseif MsgType.PUBLISH == cType then  --发布消息
        parsePUBLISH(pBinary, tInfo)
    elseif MsgType.PUBACK == cType then   --发布确认
        parsePUBACK(pBinary, tInfo)
    elseif MsgType.PUBREC == cType then   --发布接收（有保证的交付第1部分）
        parsePUBREC(pBinary, tInfo)
    elseif MsgType.PUBREL == cType then   --发布释放（有保证的交付第2部分）
        parsePUBREL(pBinary, tInfo)
    elseif MsgType.PUBCOMP == cType then  --发布完成（有保证的交付第3部分）
        parsePUBCOMP(pBinary, tInfo)
    elseif MsgType.SUBSCRIBE == cType then    --客户端订阅请求
        parseSUBSCRIBE(pBinary, tInfo)
    elseif MsgType.SUBACK == cType then   --订阅确认
        parseSUBACK(pBinary, tInfo)
    elseif MsgType.UNSUBSCRIBE == cType then  --客户端取消订阅请求
        parseUNSUBSCRIBE(pBinary, tInfo)
    elseif MsgType.UNSUBACK == cType then --取消订阅确认
        parseUNSUBACK(pBinary, tInfo)
    elseif MsgType.PINGREQ == cType then  --PING请求
        parsePINGREQ(pBinary, tInfo)
    elseif MsgType.PINGRESP == cType then --PING回复
        parsePINGRESP(pBinary, tInfo)
    elseif MsgType.DISCONNECT == cType then   --客户端断开连接
        parseDISCONNECT(pBinary, tInfo)
    else 
        assert(false)
    end   
    
    return tInfo
end

local function createHead(msgtype, dup, qos, retain, lens)
    local cType = (msgtype << 4) | (dup << 3) | (qos << 1) | retain
    
    local tHead = {}
    table.insert(tHead, string.pack("B", cType))
    local cDigit = 0
    repeat
       cDigit = lens % 128
       lens =  math.floor(lens / 128)
       if lens > 0 then
            cDigit = cDigit | 0x80
       end
       
       table.insert(tHead, string.pack("B", cDigit))
       print(lens)
    until(lens <= 0)
    
    return table.concat(tHead, "")
end

--连接确认
function mqtt.CONNACK(rtnCode)
    pWBinary:reSetWrite()
   
    local vHead = string.pack("B", 0)
    vHead = vHead .. string.pack("B", rtnCode)
    local fHead = createHead(MsgType.CONNACK, 0, 0, 0, #vHead)
    
    pWBinary:setByte(fHead, #fHead)
    pWBinary:setByte(vHead, #vHead)
    
    return pWBinary
end

return mqtt
