--[[
C++全局函数
--]]

require("macros")
local serialize = require("serialize")
local humble = {}
local string = string
local pWorkerMgr = g_pWorkerMgr
local pNet = g_pNetWorker
local pSender = g_pSender
local pEmail = g_pEmail
local newLuaTask = newLuaTask
local ChanNam = ChanNam

--网络
function humble.tcpListen(usSockType, strHost, usPort)
    pNet:tcpListen(usSockType, strHost, usPort)
end
function humble.addTcpLink(usSockType, strHost, usPort)
    pNet:addTcpLink(usSockType, strHost, usPort)
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
--tsock: {{sock,session},...}
function humble.broadCast(tSock, strBuf)
    pSender:broadCast(tSock, strBuf, string.len(strBuf))
end

--邮件
function humble.sendMail(strMail)
    pEmail:sendMail(strMail)
end

--chan注册
function humble.getSendChan(strChanNam, strTaskNam)
    return pWorkerMgr:getSendChan(strChanNam, strTaskNam)
end
function humble.getRecvChan(strChanNam, strTaskNam)
    return pWorkerMgr:getRecvChan(strChanNam, strTaskNam)
end
function humble.regChan(strChanNam, uiCount)
    pWorkerMgr:regChan(strChanNam, uiCount)
end
function humble.regTask(strNam)
    return pWorkerMgr:regTask(strNam, newLuaTask())
end

return humble
