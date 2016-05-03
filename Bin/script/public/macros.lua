--[[
宏定义
--]]

require("utile")
local table = table

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

ChanNam = {
    CloseSock = "closesockchan",
    Timer = "timerchan",
}

SockType = {
    "Tcp",
}
SockType = table.enum(SockType, 1)
