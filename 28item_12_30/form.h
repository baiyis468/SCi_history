#ifndef FORM_H
#define FORM_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QPinchGesture>

class Form : public QGraphicsView
{
    Q_OBJECT

public:
    explicit Form(QWidget *parent = nullptr);
    ~Form();

protected:
    void resizeEvent(QResizeEvent *event) override;        // 处理视图大小调整
    bool event(QEvent *event) override;                    // 处理手势事件

    void mousePressEvent(QMouseEvent *event) override;  // 鼠标按下事件
    void mouseMoveEvent(QMouseEvent *event) override;   // 鼠标移动事件
    void mouseReleaseEvent(QMouseEvent *event) override; // 鼠标释放事件

private:
    QGraphicsScene *scene;
    QGraphicsPixmapItem *pixmapItem;
    QPixmap pixmap;
    bool isDragging;                         // 是否正在拖动
    QPointF lastPos;                         // 鼠标按下的位置

    double currentScale;  // 当前缩放比例

};

#endif // FORM_H
