--[[
C++全局函数
--]]

require("macros")
local serialize = require("serialize")
local humble = {}
local string = string
local pWorkerMgr = g_pWorkerMgr
local pSender = g_pSender
local pEmail = g_pEmail
local newLuaTask = newLuaTask
local ChanNam = ChanNam

--网络
function humble.closeSock(sock, uiSession)
    local pChan = humble.getChan(ChanNam.CloseSock)
    if not pChan then
        return
    end
    
    pChan:Send(serialize.pack({sock, uiSession}))
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
function humble.regSendChan(strChanNam, strTaskNam, uiCount)
    return pWorkerMgr:regSendChan(strChanNam, strTaskNam, uiCount)
end
function humble.regRecvChan(strChanNam, strTaskNam, uiCount)
    return pWorkerMgr:regRecvChan(strChanNam, strTaskNam, uiCount)
end
function humble.getChan(strChanNam)
    return pWorkerMgr:getChan(strChanNam)
end
function humble.regTask(strNam)
    return pWorkerMgr:regTask(strNam, newLuaTask())
end

return humble
