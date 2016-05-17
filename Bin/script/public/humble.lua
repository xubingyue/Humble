--[[
C++全局函数
--]]

local serialize = require("serialize")
local string = string
local pWorkerMgr = g_pWorkerMgr
local pNet = g_pNetWorker
local pSender = g_pSender
local pEmail = g_pEmail
local pNetParser = g_pNetParser
local newLuaTask = newLuaTask

local humble = {}

--网络
function humble.setMaxLoad(uiCount)   
    pNet:setMaxLoad(uiCount)
end
function humble.getCurLoad()   
    return pNet:getCurLoad()
end
function humble.setParser(usSockType, strName)
    return pNetParser:setParser(usSockType, strName)
end
function humble.addListener(usSockType, strHost, usPort)
    return pNet:addListener(usSockType, strHost, usPort)
end
function humble.delListener(uiID)   
    pNet:delListener(uiID)
end
function humble.addTcpLink(usSockType, strHost, usPort)
    return pNet:addTcpLink(usSockType, strHost, usPort)
end
function humble.delTcpLink(uiID)   
    pNet:delTcpLink(uiID)
end
function humble.closeSock(sock, uiSession)   
    pNet:closeSock(sock, uiSession)
end
function humble.closeByType(usType)   
    pNet:closeByType(usType)
end
function humble.Send(sock, uiSession, strBuf)
    pSender:Send(sock, uiSession, strBuf, string.len(strBuf))
end
function humble.SendB(sock, uiSession, pBinary)
    pSender:sendB(sock, uiSession, pBinary)
end
--tsock: {{sock,session},...}
function humble.broadCast(tSock, strBuf)
    if 0 == #tSock then
        return
    end
    
    pSender:broadCast(tSock, strBuf, string.len(strBuf))
end
function humble.broadCastB(tSock, pBinary)
    if 0 == #tSock then
        return
    end
    
    pSender:broadCast(tSock, pBinary)
end

--邮件
function humble.sendMail(strMail)
    pEmail:sendMail(strMail)
end

function humble.getChan(strTaskNam)
    return pWorkerMgr:getChan(strTaskNam)
end
function humble.regTask(strTaskNam)
    return pWorkerMgr:regTask(strTaskNam, newLuaTask())
end

return humble
