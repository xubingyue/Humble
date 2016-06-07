
require("macros")
local msgtype = require("msgtype")
local utile = require("utile")
local humble = require("humble")
local httpd = require("httpd")
local websock = require("websock")
local mqtt = require("mqtt")
local def = require("def")
local tcp1 = require("tcp1")
local tcp2 = require("tcp2")
local cbfuncs = require("cbfuncs")
local cjson = require("cjson")
local table = table
local string = string
local pChan = g_pChan

if not g_tSock then
    g_tSock = {}
end
local tSock = g_tSock

function initTask()
    
end

local function callStr(strComm)
    local Rtn, strMsg = load(strComm)    
    if Rtn then
        Rtn, strMsg = pcall(Rtn)
        if Rtn then            
            return strMsg
        end
    end
    
    showMsg(strMsg, #strMsg)
    
    return nil
end

local function sendMsg(strParser, strComm)
    if not tSock.sock then
        local strMsg = "not linked."
        showMsg(strMsg, #strMsg)
        
        return
    end
    
    local pBinary = callStr(strComm)
    if not pBinary then
        return
    end
    
    humble.sendB(tSock.sock, tSock.session, pBinary)    
end

local function readMsg(strName, buffer)
    if "tcp1" == strName or "tcp2" == strName or "default" == strName then
        showMsg(buffer, #buffer)
        return
    end
    
    local strMsg = cjson.encode(buffer)
    showMsg(strMsg, #strMsg)
end

function runTask()
    local varRecv = pChan:Recv()
    local va0, va1, va2, va3, va4, va5 = utile.unPack(varRecv)
    if msgtype.tick == va0 then
        
    elseif msgtype.onesec == va0 then
        cbfuncs.onDelayEvent()
    elseif msgtype.link == va0 then
        tSock.sock = va1
        tSock.session = va2
        local strMsg = string.format("tcp linked, sock: %d, session %d", va1, va2)
        showMsg(strMsg, #strMsg)
    elseif msgtype.close == va0 then
        tSock = {}
        local strMsg = string.format("tcp closed, sock: %d, session %d", va1, va2)
        showMsg(strMsg, #strMsg)
    elseif msgtype.send == va0 then
        sendMsg(va1, va2)
    elseif msgtype.read == va0 then
        readMsg(va1, va2)
    else
        local strMsg = string.format("unknown message type: %s", tostring(va0))
        showMsg(strMsg, #strMsg)
    end
end

function destroyTask()

end
