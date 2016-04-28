--[[
服务初始化
--]]

local strpubdir = string.format("%s%s%s", g_strScriptPath, "public", "/")
package.path = string.format("%s;%s?.lua", package.path, strpubdir)

require("macros")
local pWorkerMgr = g_pWorkerMgr
local pNetWorker = g_pNetWorker
local pSender = g_pSender
local pEmail = g_pEmail
local pLog = g_pLog
local loglv = loglv
local netLuaTask = netLuaTask

--log
pLog:setPriority(loglv.debug) --日志级别
pLog:setLogFile(string.format("%slog/%s", g_strProPath, "log.txt"))
pLog:Open()

--email
pEmail:setServer("smtp.163.com")
--<!--类型 1：LOGIN  2：PLAIN-->
pEmail:setAuthType(2)
pEmail:setSender("xxxxx@163.com")
pEmail:setUserName("UserName")
pEmail:setPassWord("PassWord")

--tick
pNetWorker:setTick(10) --定时器触发间隔

--worker
pWorkerMgr:setThreadNum(8) --工作线程数

--任务注册
pWorkerMgr:regTask("ts_timer", netLuaTask())
