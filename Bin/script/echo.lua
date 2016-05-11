local strpubdir = string.format("%s%s%s", g_strScriptPath, "public", "/")
package.path = string.format("%s;%s?.lua", package.path, strpubdir)

require("macros")
local utile = require("utile")
local humble = require("humble")
local tcp = require("tcp")
local httpd = require("httpd")
local table = table
local string = string

if not g_pBinary then
    g_pBinary = CBinary()
end
local pBinary = g_pBinary

function initTask()
    
end

function runTask(pChan)
        --[[local varRecv = pChan:Recv()
        local sock, uiSession, tInfo = table.unpack(utile.unPack(varRecv))
        table.print(tInfo)
        local strResp = httpd.Response(200, "hello http")
        humble.Send(sock, uiSession, strResp)--]]
        local varRecv = pChan:Recv()
        local sock, uiSession, iProtocol, strMsg = table.unpack(utile.unPack(varRecv))
        tcp.creatPack(pBinary, iProtocol, strMsg)
        humble.SendB(sock, uiSession, pBinary)
end

function destroyTask()
    
end
