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
    local sock, uiSession, strMsg = utile.unPack(varRecv)
    
    --http
    --table.print(strMsg)
    --local pWBinary = httpd.Response(200, "hello word")
    --humble.SendB(sock, uiSession, pWBinary)
    
    --tcp1 tcp1
    humble.Send(sock, uiSession, strMsg)
    
    --websock
    --table.print(strMsg)
    --local pWBinary = websock.Text(strMsg.info)
    --humble.SendB(sock, uiSession, pWBinary)
    
    --mqtt
    --[[print(os.date())
    table.print(strMsg)
    local pWBinary
    if strMsg.fhead.type == mqtt.Type.CONNECT then
        pWBinary = mqtt.CONNACK(0)
    end
    
    if strMsg.fhead.type == mqtt.Type.SUBSCRIBE then
        local tTopic = strMsg.payload
        local tQos = {}
        for _, val in pairs(tTopic) do
            table.insert(tQos, val)
        end
        
        pWBinary = mqtt.SUBACK(strMsg.vhead.msgid, tQos)
    end--]]

    --humble.SendB(sock, uiSession, pWBinary)
end

function destroyTask()

end
