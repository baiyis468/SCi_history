#ifndef MAPWIDGET_H
#define MAPWIDGET_H

#include <QWidget>
#include <QPixmap>
#include <QPointF>
#include <QWheelEvent>
#include <QMouseEvent>

namespace Ui {
class MapWidget;
}

class MapWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MapWidget(QWidget *parent = nullptr);
    ~MapWidget();

protected:
    void paintEvent(QPaintEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    void adjustImage();  // 调整图片显示状态
    void resetToInitialState(); // 重置到初始状态
    void clampImagePosition(); // 确保图片位置在合理范围内

    Ui::MapWidget *ui;
    QPixmap m_originalImage;   // 原始图片
    QPixmap m_displayImage;    // 当前显示的图片
    double m_scaleFactor;      // 当前缩放因子
    double m_minScale;         // 最小缩放因子(1.0)
    double m_maxScale;         // 最大缩放因子(4.0)
    QPointF m_imagePos;        // 图片在窗口中的位置
    QPointF m_initialImagePos; // 初始图片位置
    double m_initialScale;     // 初始缩放因子

    // 拖拽相关变量
    bool m_dragging;           // 是否正在拖拽
    QPoint m_dragStartPos;     // 拖拽起始点（鼠标位置）
    QPointF m_dragStartImagePos; // 拖拽起始点（图片位置）
};

#endif // MAPWIDGET_H
