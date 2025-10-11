#include "form.h"
#include <QWheelEvent>
#include <QPinchGesture>
#include <QImageReader>
#include <QGestureEvent>

Form::Form(QWidget *parent)
    : QGraphicsView(parent),
    scene(new QGraphicsScene(this)),
    pixmapItem(nullptr),
    isDragging(false),
    currentScale(1.0)  // 初始比例为 1
{
    // 设置一些渲染提示
    setRenderHint(QPainter::Antialiasing);  // 启用抗锯齿
    setRenderHint(QPainter::SmoothPixmapTransform);  // 启用平滑缩放

    setScene(scene);  // 将scene添加到view中

    // 加载图片（请根据实际路径替换）
    pixmap.load(":/new/prefix1/C:/Users/86135/Desktop/map.jpg");

    // 创建pixmap项，将图片添加到场景
    pixmapItem = scene->addPixmap(pixmap);
    pixmapItem->setTransformationMode(Qt::SmoothTransformation);

    // 设置场景的初始视图大小
    setSceneRect(pixmapItem->boundingRect());

    // 启用触摸事件来支持手势
    grabGesture(Qt::PinchGesture);  // 启用捏合手势
}

Form::~Form()
{
    delete scene;
}

void Form::resizeEvent(QResizeEvent *event)
{
    QGraphicsView::resizeEvent(event);

    // 当视图大小改变时，保持图片铺满整个视图
    QSize viewSize = event->size();
    setSceneRect(0, 0, viewSize.width(), viewSize.height());
    pixmapItem->setPixmap(pixmap.scaled(viewSize, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
}

bool Form::event(QEvent *event)
{
    qDebug()<<currentScale;
    // 处理捏合手势
    if (event->type() == QEvent::Gesture) {
        QGestureEvent *gestureEvent = static_cast<QGestureEvent *>(event);
        QPinchGesture *pinchGesture = static_cast<QPinchGesture *>(gestureEvent->gesture(Qt::PinchGesture));

        if (pinchGesture) {
            qreal scaleFactor = pinchGesture->scaleFactor();
            if(scaleFactor > 1 && currentScale < 5 || scaleFactor < 1 && currentScale > 1)// 放大操作 并且 比例还能放大  或者  缩小操作 并且 比例还能缩小
            {
                scale(scaleFactor, scaleFactor);  // 缩放视图
                currentScale *= scaleFactor;
            }
                return true;
        }
    }

    return QGraphicsView::event(event);  // 交给基类处理其他事件
}

void Form::mousePressEvent(QMouseEvent *event)
{
    // 判断鼠标是否点击在图片区域内
    if (pixmapItem->contains(mapToScene(event->pos()))) {
        // 如果点击了图片区域，启动拖动
        isDragging = true;
        lastPos = event->pos();  // 记录当前鼠标按下的位置
    }
}

// 鼠标移动事件
void Form::mouseMoveEvent(QMouseEvent *event)
{
    if (isDragging) {
        // 计算鼠标移动的偏移量
        QPointF delta = mapToScene(event->pos()) - mapToScene(lastPos.toPoint());

        // 获取当前图片的边界
        QRectF pixmapRect = pixmapItem->boundingRect();

        // 计算图片的缩放后的宽高
        qreal scaledWidth = pixmapRect.width() * currentScale;
        qreal scaledHeight = pixmapRect.height() * currentScale;

        // 获取视图的可视区域（视口大小）
        QRectF viewRect = this->viewport()->rect();

        // 计算图片当前位置
        QPointF newPos = pixmapItem->pos() + delta;

        // 限制图片的边界，确保它不会拖出视图的边缘
        if (newPos.x() > 0) {
            newPos.setX(0); // 限制左边界
        }
        if (newPos.y() > 0) {
            newPos.setY(0); // 限制上边界
        }
        if (newPos.x() + scaledWidth < viewRect.width()) {
            newPos.setX(viewRect.width() - scaledWidth); // 限制右边界
        }
        if (newPos.y() + scaledHeight < viewRect.height()) {
            newPos.setY(viewRect.height() - scaledHeight); // 限制下边界
        }

        // 移动图片至新位置
        pixmapItem->setPos(newPos);

        // 更新鼠标按下位置
        lastPos = event->pos();
    }
}


// 鼠标释放事件
void Form::mouseReleaseEvent(QMouseEvent *event)
{
    // 停止拖动
    isDragging = false;
}
