#pragma once
#include <QNetworkAccessManager>
#include <unordered_map>

struct IMAGE_CACHE_MANAGER
{
    QNetworkAccessManager *image_network_manager;
    std::unordered_map<QString, QPixmap> image_cacher;
    QString src;
};