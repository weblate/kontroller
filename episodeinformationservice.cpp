#include "episodeinformationservice.h"
#include "kodiclient.h"
#include "utils.h"
#include "videocontrol.h"
#include "kodifile.h"

EpisodeInformationService::EpisodeInformationService(QObject *parent) : QObject(parent)
{

}

QString EpisodeInformationService::plot() const
{
    return plot_;
}

double EpisodeInformationService::rating() const
{
    return rating_;
}

int EpisodeInformationService::runtime() const
{
    return runtime_;
}

int EpisodeInformationService::episode() const
{
    return episode_;
}

QString EpisodeInformationService::showTitle() const
{
    return showTitle_;
}

QString EpisodeInformationService::file() const
{
    return file_;
}

QString EpisodeInformationService::fanart() const
{
    return fanart_;
}

QString EpisodeInformationService::thumbnail() const
{
    return thumbnail_;
}

QDateTime EpisodeInformationService::lastplayed() const
{
    return lastplayed_;
}

int EpisodeInformationService::season() const
{
    return season_;
}

QString EpisodeInformationService::art() const
{
    return art_;
}

int EpisodeInformationService::episodeId() const
{
    return episodeId_;
}

QString EpisodeInformationService::title() const
{
    return title_;
}

void EpisodeInformationService::setPlot(QString plot)
{
    if (plot_ == plot)
        return;

    plot_ = plot;
    emit plotChanged(plot);
}

void EpisodeInformationService::setRating(double rating)
{
    if (rating_ == rating)
        return;

    rating_ = rating;
    emit ratingChanged(rating);
}

void EpisodeInformationService::setRuntime(int runtime)
{
    if (runtime_ == runtime)
        return;

    runtime_ = runtime;
    emit runtimeChanged(runtime);
}

void EpisodeInformationService::setEpisode(int episode)
{
    if (episode_ == episode)
        return;

    episode_ = episode;
    emit episodeChanged(episode);
}

void EpisodeInformationService::setShowTitle(QString showTitle)
{
    if (showTitle_ == showTitle)
        return;

    showTitle_ = showTitle;
    emit showTitleChanged(showTitle);
}

void EpisodeInformationService::setFile(QString file)
{
    if (file_ == file)
        return;

    file_ = file;
    emit fileChanged(file);
}

void EpisodeInformationService::setFanart(QString fanart)
{
    if (fanart_ == fanart)
        return;

    fanart_ = fanart;
    emit fanartChanged(fanart);
}

void EpisodeInformationService::setThumbnail(QString thumbnail)
{
    if (thumbnail_ == thumbnail)
        return;

    thumbnail_ = thumbnail;
    emit thumbnailChanged(thumbnail);
}

void EpisodeInformationService::setLastplayed(QDateTime lastplayed)
{
    if (lastplayed_ == lastplayed)
        return;

    lastplayed_ = lastplayed;
    emit lastplayedChanged(lastplayed);
}

void EpisodeInformationService::setSeason(int season)
{
    if (season_ == season)
        return;

    season_ = season;
    emit seasonChanged(season);
}

void EpisodeInformationService::setArt(QString art)
{
    if (art_ == art)
        return;

    art_ = art;
    emit artChanged(art);
}

void EpisodeInformationService::refresh()
{
    if(episodeId_ != 0)
    {
        QJsonObject parameters;
        parameters["episodeid"] = episodeId_;
        QJsonArray properties;
        properties.append(QString("title"));
        properties.append(QString("plot"));
        properties.append(QString("rating"));
        properties.append(QString("runtime"));
        properties.append(QString("episode"));
        properties.append(QString("showtitle"));
        properties.append(QString("file"));
        properties.append(QString("fanart"));
        properties.append(QString("thumbnail"));
        properties.append(QString("lastplayed"));
        properties.append(QString("season"));
        properties.append(QString("art"));

        properties.append(QString("resume"));
        properties.append(QString("cast"));
        properties.append(QString("originaltitle"));
        properties.append(QString("dateadded"));

        parameters["properties"] = properties;
        QJsonRpcMessage message = QJsonRpcMessage::createRequest("VideoLibrary.GetEpisodeDetails", parameters);
        auto reply = KodiClient::current().send(message);
        connect(reply, &QJsonRpcServiceReply::finished, this, &EpisodeInformationService::handleRefresh_);
    }
}

void EpisodeInformationService::setTitle(QString title)
{
    if (title_ == title)
        return;

    title_ = title;
    emit titleChanged(title);
}

void EpisodeInformationService::playFile()
{
    KodiFile file;
    file.setFiletype("episode");
    file.setType("episode");
    file.setFile(QString::number(episodeId_));
    file.setLabel(title());
    VideoControl ctrl;
    ctrl.playFile(&file);
}

void EpisodeInformationService::handleRefresh_()
{
    auto reply = dynamic_cast<QJsonRpcServiceReply*>(sender());
    if(reply)
    {
        auto response = reply->response().result().toObject();
        if(response.isEmpty())
            return;
        auto details = response.value("episodedetails").toObject();
        if(details.isEmpty())
            return;
        auto art = details.value("art").toObject();
        Q_UNUSED(art);
        setTitle(details.value("title").toString());
        setPlot(details.value("plot").toString().replace("\r", ""));
        setRating(details.value("rating").toDouble());
        setRuntime(details.value("runtime").toDouble());
        setEpisode(details.value("episode").toDouble());
        setShowTitle(details.value("showtitle").toString());
        setFile(details.value("file").toString());
        setFanart(getImageUrl(details.value("fanart").toString()).toString());
        setThumbnail(getImageUrl(details.value("thumbnail").toString()).toString());
        setLastplayed(QDateTime::fromString(details.value("lastplayed").toString(), Qt::ISODate));
        setSeason(details.value("season").toDouble());
    }
}

void EpisodeInformationService::setEpisodeId(int episodeId)
{
    if (episodeId_ == episodeId)
        return;

    episodeId_ = episodeId;
    emit episodeIdChanged(episodeId);
}
