--[[
邮件发送
--]]

local table = table
local assert = assert
local setmetatable = setmetatable
local cjson = require("cjson")
local humble = require("humble")
local sendMail = humble.sendMail

Mail = {}
Mail.__index = Mail

function Mail:new()
    local self = {}
    setmetatable(self, Mail)
    
    self.Subject = ""--主题
    self.StrMsg = ""--字符串内容
    self.HtmlStrMsg = ""--Html字符串内容
    self.HtmlFileMsg = ""--Html文件内容
    self.Recipient = {}--接收者
    self.Attach = {}--附件 
    
    return self
end

--清理
function Mail:Clear()
    self.Subject = ""
    self.StrMsg = ""
    self.HtmlStrMsg = ""
    self.HtmlFileMsg = ""
    self.Recipient = {}
    self.Attach = {} 
end

--发送
function Mail:Send()
    local strJson = cjson.encode(self)
    sendMail(strJson)
end

--主题
function Mail:setSubject(strSubject)
    self.Subject = strSubject
end

--消息
function Mail:setStrMsg(strMsg)
    self.StrMsg = strMsg
end

function Mail:setHtmlMsg(strHtmlMsg)
    self.HtmlStrMsg = strHtmlMsg
end

function Mail:setHtmlFileMsg(strHtmlFile)
    self.HtmlFileMsg = strHtmlFile
end

--接收者
function Mail:addRecipient(strAddr)
    for _, val in pairs(self.Recipient) do
        if val == strAddr then
            return
        end
    end
    
    table.insert(self.Recipient, strAddr)
end

function Mail:removeRecipient(strAddr)
    for key, val in pairs(self.Recipient) do
        if val == strAddr then
            table.remove(self.Recipient, key)
            return
        end
    end
end

function Mail:clearRecipient()
    self.Recipient = {}
end

--附件
function Mail:addAttach(strFile)
    for _, val in pairs(self.Attach) do
        if val == strFile then
            return
        end
    end
    
    table.insert(self.Attach, strFile)
end

function Mail:removeAttach(strFile)
    for key, val in pairs(self.Attach) do
        if val == strFile then
            table.remove(self.Attach, key)
            return
        end
    end
end

function Mail:clearAttach()
    self.Attach = {}
end
