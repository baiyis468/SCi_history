#ifndef RELICS_H
#define RELICS_H

#include <QObject>
#include <QWidget>

class relics: public QObject
{
    Q_OBJECT
public:
    relics(QString& m_ID,QString& m_model_3D,QString& m_name);
private:
    QString m_ID;
    QString model_3D;
    QString m_name;
};

#endif // RELICS_H
