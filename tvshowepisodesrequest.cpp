#include "tvshowepisodesrequest.h"
#include "kodiclient.h"
#include "kodifile.h"


TvShowEpisodesRequest::TvShowEpisodesRequest(QObject *parent) : QObject(parent)
{

}

TvShowEpisodesRequest::~TvShowEpisodesRequest()
{
    for(KodiFile* file : episodes)
        file->deleteLater();
    episodes.clear();
}

void TvShowEpisodesRequest::start(int tvshowId, int season)
{
    QJsonObject parameters;
    parameters.insert("season", season);
    parameters.insert("tvshowid", tvshowId);
    QJsonObject sort;
    sort.insert("order", "ascending");
    sort.insert("method", "episode");
    parameters.insert("sort", sort);
    QJsonRpcMessage message = QJsonRpcMessage::createRequest("VideoLibrary.GetEpisodes", parameters);
    QJsonRpcServiceReply* reply = KodiClient::current().send(message);
    if(reply)
        connect(reply, &QJsonRpcServiceReply::finished, this, &TvShowEpisodesRequest::parseEpisodesResult_);
}

void TvShowEpisodesRequest::parseEpisodesResult_()
{
    QJsonRpcServiceReply* reply = dynamic_cast<QJsonRpcServiceReply*>(sender());
    if(reply)
    {
        QJsonRpcMessage response = reply->response();
        QJsonObject obj = response.toObject();
        if(obj.find("result") != obj.end())
        {
            QJsonValue result = obj.take("result");
            if(result.type() == QJsonValue::Object)
            {
                QJsonValue files;
                files = result.toObject().take("episodes");
                if(files.type() == QJsonValue::Array)
                {
                    QJsonArray res = files.toArray();
                    for(QJsonArray::const_iterator it = res.begin(); it != res.end(); ++it)
                    {
                        KodiFile* file = new KodiFile();
                        if((*it).type() == QJsonValue::Object)
                        {
                            QJsonObject obj = (*it).toObject();
                            QJsonValue val = obj.value("label");
                            if(val.type() == QJsonValue::String)
                                file->setLabel(val.toString());
                            val = obj.value("episodeid");
                            if(val.type() == QJsonValue::Double)
                                file->setFile(QString::number(val.toDouble()));
                            file->setFiletype("episode");
                            file->setType("episode");
                            episodes.push_back(file);
                        }
                    }
                }
            }
        }
        success = true;
    }
    emit finished();
}
