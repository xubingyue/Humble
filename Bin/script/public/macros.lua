--[[
宏定义
--]]

local table = table
local assert = assert

function table.enum(tMsg, iBegin) 
    assert("table" == type(tMsg))    
    local tEnum = {} 
    local iEnumIndex = iBegin or 0 
    for key, val in pairs(tMsg) do 
        tEnum[val] = iEnumIndex + key - 1
    end 
    
    return tEnum 
end 

--无效ID
Invalid_ID = -1
Invalid_Sock = -1

ErrCode = {
    "faile",
    "ok",
    "error",
}
ErrCode = table.enum(ErrCode, -1)

--文本日志级别
LogLV = {
    "Err",
    "Warn",
    "Info",
    "Debug",
}
LogLV = table.enum(LogLV, 1)
