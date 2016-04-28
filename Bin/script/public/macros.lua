--[[
宏定义
--]]

require("utile")
local table = table

--无效ID
invalid_id = -1
invalid_sock = -1

errcode = {
    "faile",
    "ok",
    "error",
}
errcode = table.enum(errcode, -1)

--文本日志级别
loglv = {
    "error",
    "warn",
    "info",
    "debug",
}
loglv = table.enum(loglv, 1)

channam = {
    closesock = "sockclosechan",
    timer = "timerchan",
}

socktype = {
    "tcp",
}
socktype = table.enum(socktype, 1)
