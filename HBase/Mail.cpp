
#include "Mail.h"
#include "Funcs.h"
#include "json/json.h"

H_BNAMSP

SINGLETON_INIT(CMail)
CMail objMail;

CMail::CMail(void) : m_bSet(false)
{

}

CMail::~CMail(void)
{

}

void CMail::sendMail(const char *pszMail)
{
    std::string *pstrMsg = newT();
    *pstrMsg = pszMail;

    addTask(pstrMsg);
}

int CMail::parseMail(std::string *pstrMail)
{
    Json::Reader objJsonReader;
    Json::Value objJsonRoot;

    if (!objJsonReader.parse(*pstrMail, objJsonRoot, false))
    {
        H_Printf("parse string <%s> error.", pstrMail->c_str());

        return H_RTN_FAILE;
    }

    //主题
    std::string strSubject = objJsonRoot["Subject"].asString();
    if (!m_objMailer.setsubject(strSubject))
    {
        H_Printf("%s", "subject is empty.");

        return H_RTN_FAILE;
    }
    //字符串消息
    std::string strStrMsg = objJsonRoot["StrMsg"].asString();
    if (!strStrMsg.empty())
    {
        (void)m_objMailer.setmessage(strStrMsg);
    }
    //html字符串消息
    std::string strHtmlStrMsg = objJsonRoot["HtmlStrMsg"].asString();
    if (!strHtmlStrMsg.empty())
    {
        (void)m_objMailer.setmessageHTML(strHtmlStrMsg);
    }
    //html文件消息
    std::string strHtmlFileMsg = objJsonRoot["HtmlFileMsg"].asString();
    if (!strHtmlFileMsg.empty())
    {
        (void)m_objMailer.setmessageHTMLfile(strHtmlFileMsg);
    }

    //接收者
    Json::Value objRecipient = objJsonRoot["Recipient"];
    if (objRecipient.empty())
    {
        H_Printf("%s", "recipient is empty.");

        return H_RTN_FAILE;
    }
    for (Json::ArrayIndex i = 0; i < objRecipient.size(); i++)
    {
        std::string strRecipient = objRecipient[i].asString();
        if (!strRecipient.empty())
        {
            (void)m_objMailer.addrecipient(strRecipient);
        }
    }

    //附件
    Json::Value objAttach = objJsonRoot["Attach"];
    if (objAttach.empty())
    {
        return H_RTN_OK;
    }

    for (Json::ArrayIndex i = 0; i < objAttach.size(); i++)
    {
        std::string strFile = objAttach[i].asString();
        if (H_RTN_OK != H_FileExist(strFile.c_str()))
        {
            H_Printf("file %s not exist.", strFile.c_str());

            return H_RTN_FAILE;
        }

        (void)m_objMailer.attach(strFile);
    }

    return H_RTN_OK;
}

void CMail::runTask(std::string *pMsg)
{
    m_objMailer.reset();
    if (H_RTN_OK == parseMail(pMsg))
    {
        m_objMailer.send();
        H_Printf("%s", m_objMailer.response().c_str());
    }
        
}

void CMail::setServer(const std::string strServer)
{
    m_bSet = true;
    (void)m_objMailer.setserver(strServer);
}

void CMail::setAuthType(const int uiType)
{
    m_bSet = true;
    m_objMailer.authtype((enum jwsmtp::mailer::authtype)uiType);
}

void CMail::setSender(const std::string strSender)
{
    m_bSet = true;
    (void)m_objMailer.setsender(strSender);
}

void CMail::setUserName(const std::string strUserName)
{
    m_bSet = true;
    m_objMailer.username(strUserName);
}

void CMail::setPassWord(const std::string strPassWord)
{
    m_bSet = true;
    m_objMailer.password(strPassWord);
}

bool CMail::getSetted(void)
{
    return m_bSet;
}

H_ENAMSP
