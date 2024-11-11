#ifndef DATA_H
#define DATA_H

#include <QString>
#include <QIcon>
#include <QUuid>
#include <QDateTime>
#include <QFile>
#include <QDebug>
#include <QFileInfo>
namespace model
{
////////////////////////////////////////////////////////
/// 工具函数，后续很多模块可能都要用到
////////////////////////////////////////////////////////

static inline QString getFileName(const QString& path)
{
    QFileInfo fileInfo(path);
    return fileInfo.fileName();
}

//封装一个"宏"作为打印日志的方式
#define TAG (QString("[%1 :%2]").arg(model::getFileName(__FILE__),QString::number(__LINE__)))
#define LOG() qDebug().noquote() << TAG

//防止函数重复定义,static 或者 inline 必有一个
//秒级时间戳转换格式化时间
static inline QString formatTime(int64_t timestamp)
{
    //传入的是秒级时间戳
    QDateTime dateTime =  QDateTime::fromSecsSinceEpoch(timestamp);
    //把QDateTime对象转换成格式化时间
    return dateTime.toString("MM-dd HH:mm::ss");
}

//获取秒级时间戳
static inline int64_t getSecTime()
{
    return QDateTime::currentSecsSinceEpoch();
}

//把QByteArray转换成QIcon
static inline QIcon makeIcon(const QByteArray& byteArray)
{
    QPixmap pixmap;
    pixmap.loadFromData(byteArray);
    QIcon icon(pixmap);
    return icon;
}

//读写文件操作
//从指定文件中，二进制方式读取内容，再得到一个QByteArray
static inline QByteArray loadFileToByteArray(const QString& path)
{
    QFile file(path);
    bool ok = file.open(QFile::ReadOnly);
    if(!ok)
    {
        LOG()<<"文件打开失败!";
        return QByteArray();
    }
    QByteArray content = file.readAll();
    file.close();
    return content;
}

//把QByteArray中的内容写到某个指定文件
static inline void writeByteArrayToFile(const QString& path,const QByteArray& content)
{
    QFile file(path);
    bool ok = file.open(QFile::WriteOnly);
    if(!ok)
    {
        LOG()<<"文件打开失败!";
        return;
    }
    file.write(content);
    file.flush();
    file.close();
}

////////////////////////////////////////////////////////
/// 用户信息
////////////////////////////////////////////////////////
class UserInfo{
public:
    QString userId = "";     //用户编号
    QString nickname = "";   //用户昵称
    QString description = "";//用户前面
    QString phone = "";      //手机号码
    QIcon avatar;       //用户头像
};

////////////////////////////////////////////////////////
/// 消息信息
////////////////////////////////////////////////////////
enum MessageType{
    TEXT_TYPE,      //文本信息类型
    IMAGE_TYPE,     //图片信息类型
    FILE_TYPE,      //文件信息类型
    SPEECH_TYPE,    //语音信息类型
    UNKNOWN_TYPE    //未知信息类型
};

class Message{
public:
    QString messageId = "";                 //消息编号
    QString chatSessionId = "";             //消息所属会话的ID
    QString time = "";                      //消息时间，格式化时间字符串
    MessageType messageType = TEXT_TYPE;    //消息类型
    UserInfo sender;                        //消息的发送方
    //实际内容取决于messageType
    QByteArray content;                     //消息内容
    //如果是文件，包括图片文件，语音文件等，表示对应的文件ID
    QString fileId = "";                    //文件ID
    //如果是文件消息，则表示文件名
    QString fileName = "";                  //文件名称

public:
    static Message makeMessage(MessageType messageType,const QString& chatSessionId,const UserInfo& sender,
                               const QByteArray& content,const QString& extraInfo){
        if(messageType == TEXT_TYPE)
        {
            return makeTextMessage(chatSessionId,sender,content);
        }
        else if(messageType == IMAGE_TYPE)
        {
            return makeImageMessage(chatSessionId,sender,content);
        }
        else if(messageType == FILE_TYPE)
        {
            return makeFileMessage(chatSessionId,sender,content,extraInfo);
        }
        else if(messageType == SPEECH_TYPE)
        {
            return makeSpeechMessage(chatSessionId,sender,content);
        }
        else if(messageType ==UNKNOWN_TYPE)
        {
            return Message();
        }
        else
        {
            return Message();
        }
    }

private :
    static QString makeId(){
        return "M" + QUuid::createUuid().toString().sliced(25,12);//截取一部分来提高可读性
    }

    static Message makeTextMessage(const QString& chatSessionId,const UserInfo& sender,const QByteArray& content){
        Message msg;
        msg.messageId = makeId();//使用QT提供的uuid接口
        msg.chatSessionId = chatSessionId;
        msg.sender=sender;
        msg.time = formatTime(getSecTime()) ;//生成一个格式化时间
        msg.content = content;
        msg.messageType = TEXT_TYPE;

        //冗余项
        msg.fileId = "";
        msg.fileName = "";
        return msg;
    }

    static Message makeImageMessage(const QString& chatSessionId,const UserInfo& sender,const QByteArray& content){
        Message msg;
        msg.messageId = makeId();//使用QT提供的uuid接口
        msg.chatSessionId = chatSessionId;
        msg.sender=sender;
        msg.time = formatTime(getSecTime()) ;//生成一个格式化时间
        msg.content = content;
        msg.messageType = IMAGE_TYPE;

        msg.fileId = "";    //TODO
        //冗余项
        msg.fileName = "";
        return msg;
    }

    static Message makeFileMessage(const QString& chatSessionId,const UserInfo& sender,const QByteArray& content,const QString&filename){
        Message msg;
        msg.messageId = makeId();//使用QT提供的uuid接口
        msg.chatSessionId = chatSessionId;
        msg.sender=sender;
        msg.time = formatTime(getSecTime()) ;//生成一个格式化时间
        msg.content = content;
        msg.messageType = FILE_TYPE;

        msg.fileId = "";    //TODO
        msg.fileName = filename;
        return msg;
    }

    static Message makeSpeechMessage(const QString& chatSessionId,const UserInfo& sender,const QByteArray& content){
        Message msg;
        msg.messageId = makeId();//使用QT提供的uuid接口
        msg.chatSessionId = chatSessionId;
        msg.sender=sender;
        msg.time = formatTime(getSecTime()) ;//生成一个格式化时间
        msg.content = content;
        msg.messageType = SPEECH_TYPE;

        msg.fileId = "";    //TODO
        //冗余项
        msg.fileName = "";
        return msg;
    }
};

////////////////////////////////////////////////////////
/// 会话信息
////////////////////////////////////////////////////////
class ChatSessionInfo{
public:
    QString chatSessionId = "";      //会话编号
    QString charSessionName = "";    //会话名称
    Message lastMessage;            //最新消息
    QIcon avatar;                   //会话头像 个人/群组
    QString userId = "";            //对于单聊时，表示对方id，群聊时置空，不工作
};



}//end model
#endif // DATA_H
