local strpubdir = string.format("%s%s%s", g_strScriptPath, "public", "/")
package.path = string.format("%s;%s?.lua", package.path, strpubdir)

require("macros")
local utile = require("utile")
local humble = require("humble")
local httpd = require("httpd")
local websock = require("websock")
local mqtt = require("mqtt")
local table = table
local string = string
local WSCode = WSCode
local pChan = g_pChan

if not g_pBinary then
    g_pBinary = CBinary()
end
local pBinary = g_pBinary

function initTask()
    
end

function runTask()
    local varRecv = pChan:Recv()
    local sock, uiSession, strMsg = table.unpack(utile.unPack(varRecv))
    
    --http
    --table.print(strMsg)
    --local pWBinary = httpd.Response(200, "hello word")
    --humble.SendB(sock, uiSession, pWBinary)
    
    --tcp1 tcp1
    --humble.Send(sock, uiSession, strMsg)
    
    --websock
    --table.print(strMsg)
    --local pWBinary = websock.Text(strMsg.info)
    --humble.SendB(sock, uiSession, pWBinary)
    
    --mqtt
    table.print(strMsg)
    local pWBinary = mqtt.CONNACK(0)
    humble.SendB(sock, uiSession, pWBinary)
end

function destroyTask()

end
