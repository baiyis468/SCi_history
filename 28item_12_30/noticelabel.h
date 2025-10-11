#ifndef NOTICELABEL_H
#define NOTICELABEL_H

#include <QLabel>
#include <QObject>
#include<QTouchEvent>
class NoticeLabel : public QLabel
{
    Q_OBJECT
public:
    explicit NoticeLabel(QStringList strlist,QWidget *parent = nullptr);

private slots:
    void scrollText();                    //左移且获取移动的宽度。
    void nextNotice();                     //切换到下一个公告

private:
    void setCurrwidth(int curr);
    int currwidth;
    QStringList m_strlist;
    int index;
    QTimer *outTimer;
    QTimer *onTimer;
    QTimer*  goTimer;
    void getTextOverflowWidth();   // 获得溢出宽度。

    // QWidget interface
protected:
    virtual void mousePressEvent(QMouseEvent *event) override;
};


#endif // NOTICELABEL_H




