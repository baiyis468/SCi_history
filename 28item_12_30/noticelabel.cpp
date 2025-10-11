#include "noticelabel.h"
#include "noticedialog.h"

#include<QTimer>
#include <QFontMetrics>
#include <QTextLayout>
#include<QPainter>
NoticeLabel::NoticeLabel( QStringList strlist,QWidget *parent):m_strlist(strlist),index(0)
{
    setFixedSize(400, 50);
    setStyleSheet("font-size: 16px;");
    setAlignment(Qt::AlignCenter);

    setText(m_strlist[index]);

    onTimer = new QTimer(this);
    outTimer = new QTimer(this);
    goTimer = new QTimer(this);

    connect(onTimer, &QTimer::timeout, this, &NoticeLabel::getTextOverflowWidth);
    connect(outTimer, &QTimer::timeout, this, &NoticeLabel::nextNotice);
    connect(goTimer,&QTimer::timeout,this,&NoticeLabel::scrollText);
    onTimer->start(200);

}
void NoticeLabel::getTextOverflowWidth()
{
    QString text = this->text();  // 获取文本
    QFont font = this->font();    // 获取字体

    int textWidth = 0;
    QPainter painter;
    painter.setFont(font);
    textWidth = painter.fontMetrics().horizontalAdvance(text);
    int labelWidth = this->width();
    if (textWidth > labelWidth) setCurrwidth(textWidth - labelWidth);
    else setCurrwidth(0);
    setCurrwidth(currwidth+labelWidth);
    goTimer->start(1000);
    onTimer->stop();
}
void NoticeLabel::scrollText()
{
    QString currentText = text();

    // 获取当前字体
    QFont font = this->font();

    // 使用 QFontMetrics 获取字符宽度
    QFontMetrics fontMetrics(font);

    int charWidth = fontMetrics.horizontalAdvance(currentText[0]);
    if (currwidth > 0)
    {
        setCurrwidth(currwidth-charWidth);
        // 向左滚动: 将第一个字符移到最后
        setText(currentText.mid(1) + currentText[0]);
        setCurrwidth(currwidth - charWidth);
    }
    else
    {
        goTimer->stop();
        outTimer->start(1000);
    }
}

void NoticeLabel::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        NoticeDialog* notice = new NoticeDialog(m_strlist);
        notice->show();
    }
    QLabel::mousePressEvent(event);
}

void NoticeLabel::nextNotice()
{
    index = (index + 1) % m_strlist.size();
    setText(m_strlist[index]);
    outTimer->stop();
    onTimer->start(200);
}

void NoticeLabel::setCurrwidth(int curr)
{
    currwidth = curr;
}
