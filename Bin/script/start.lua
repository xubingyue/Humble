--[[
网络器服务
--]]

local strpubdir = string.format("%s%s%s", g_strScriptPath, "public", "/")
package.path = string.format("%s;%s?.lua", package.path, strpubdir)

require("macros")
local humble = require("humble")
local utile = require("utile")
local tcp = require("tcp")
local httpd = require("httpd")
local table = table
local pairs = pairs
local SockType = SockType
local pTcpBuffer = g_pTcpBuffer

if not g_tChan then
    g_tChan = {}    
end
local tChan = g_tChan

--初始化
function onStart()
    humble.tcpListen(1, "0.0.0.0", 15000)
    --humble.addTcpLink(1, "127.0.0.1", 15000)  
    
    humble.regChan("echochan", 1024)
    humble.regChan("timetick", 1024)
    
    humble.regTask("echo")
    humble.regTask("test")      
    
    tChan.echo = humble.getSendChan("echochan", "start")
end

--退出，主要清理掉连接
function onStop()
    humble.closeByType(1)
end

function onTcpLinked(sock, uiSession, usType)
    
end

function onTcpClose(sock, uiSession, usType)
    
end

function onTcpRead(sock, uiSession, usType)
    local iParsed, tInfo = httpd.parsePack(pTcpBuffer)
    if 0 ~= iParsed then
        tChan.echo:Send(utile.Pack({sock, uiSession, tInfo}))
        pTcpBuffer:delBuffer(iParsed)
    end
    --[[local iProtocol, strMsg
    local iParsed, tMsg = tcp.parsePack(pTcpBuffer)
    if 0 ~= iParsed then
        for _, val in pairs(tMsg) do
            iProtocol = val[1]
            strMsg = val[2]
            tChan.echo:Send(utile.Pack({sock, uiSession, iProtocol, strMsg}))
        end
        
        pTcpBuffer:delBuffer(iParsed)
    end--]]    
end
