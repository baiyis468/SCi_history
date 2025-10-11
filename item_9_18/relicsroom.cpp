#include "relicsroom.h"
#include<relics.h>
#include<QUrl>
#include<QtNetwork/QNetworkRequest>
#include<QtNetwork/QNetworkAccessManager>
#include<QEventLoop>
#include<QtNetwork/QRestReply>
relicsRoom::relicsRoom(QHash<QString, relics*> relicsRoom):m_relicsRoom(relicsRoom) {}
