#ifndef RELICSROOM_H
#define RELICSROOM_H
#include<QHash>
#include <QObject>
class relics;
class relicsRoom: public QObject
{
    Q_OBJECT
public:
    relicsRoom(QHash<QString, relics*> relicsRoom);
private:
    QHash<QString, relics*> m_relicsRoom;// 一个编号对应一个文物。
};

#endif // RELICSROOM_H
