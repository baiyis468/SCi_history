#include "mapwidget.h"
#include "ui_mapwidget.h"
#include <QPainter>
#include <QWheelEvent>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QDebug>
#include <QFile>
#include <QMessageBox>
#include <QCursor>

MapWidget::MapWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MapWidget)
{
    ui->setupUi(this);

    // 初始化缩放参数
    m_scaleFactor = 1.0;
    m_minScale = 1.0;   // 最小缩放比例（原始大小）
    m_maxScale = 4.0;   // 最大缩放比例（4倍）
    m_dragging = false; // 初始状态未拖拽

    // 设置背景色（图片未加载时显示）
    setStyleSheet("background-color: #2c3e50;");

    // 设置鼠标追踪
    setMouseTracking(true);

    // 直接从qrc资源加载图片
    if (m_originalImage.load("://Photos/MainSchoolMap.jpg")) {
        // 保存初始状态
        m_initialScale = m_minScale;
        m_displayImage = m_originalImage;
        resetToInitialState(); // 设置初始位置和缩放
    } else {
        // 如果图片加载失败，创建错误提示图片
        QPixmap errorImage(400, 300);
        errorImage.fill(Qt::darkRed);
        QPainter painter(&errorImage);
        painter.setPen(Qt::white);
        painter.setFont(QFont("Arial", 16));
        painter.drawText(errorImage.rect(), Qt::AlignCenter, "图片加载失败\n://Photos/MainSchoolMap.jpg");
        m_originalImage = errorImage;
        m_displayImage = errorImage;
        resetToInitialState();

        // 显示错误信息
        qWarning() << "无法加载图片资源: ://Photos/MainSchoolMap.jpg";
    }
}

MapWidget::~MapWidget()
{
    delete ui;
}

void MapWidget::resetToInitialState()
{
    if (m_originalImage.isNull()) return;

    // 重置缩放因子
    m_scaleFactor = m_initialScale;

    // 重置图片显示
    m_displayImage = m_originalImage.scaled(
        size(),
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation
        );

    // 重置图片位置
    m_imagePos = QPointF(
        (width() - m_displayImage.width()) / 2.0,
        (height() - m_displayImage.height()) / 2.0
        );

    // 保存为初始位置
    m_initialImagePos = m_imagePos;

    update();
}

void MapWidget::clampImagePosition()
{
    // 确保图片不会移出窗口可视区域
    if (m_displayImage.width() < width()) {
        m_imagePos.setX((width() - m_displayImage.width()) / 2.0);
    } else {
        // 图片宽度大于窗口宽度
        if (m_imagePos.x() > 0) m_imagePos.setX(0);
        else if (m_imagePos.x() + m_displayImage.width() < width()) {
            m_imagePos.setX(width() - m_displayImage.width());
        }
    }

    if (m_displayImage.height() < height()) {
        m_imagePos.setY((height() - m_displayImage.height()) / 2.0);
    } else {
        // 图片高度大于窗口高度
        if (m_imagePos.y() > 0) m_imagePos.setY(0);
        else if (m_imagePos.y() + m_displayImage.height() < height()) {
            m_imagePos.setY(height() - m_displayImage.height());
        }
    }
}

void MapWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);

    // 绘制背景
    painter.fillRect(rect(), QBrush(QColor("#2c3e50")));

    // 绘制图片（如果已加载）
    if (!m_displayImage.isNull()) {
        painter.drawPixmap(m_imagePos.toPoint(), m_displayImage);
    }
}

void MapWidget::wheelEvent(QWheelEvent *event)
{
    if (m_originalImage.isNull()) return;

    // 计算缩放比例
    double scaleDelta = event->angleDelta().y() > 0 ? 1.1 : 0.9;
    double newScale = m_scaleFactor * scaleDelta;

    // 限制缩放范围
    if (newScale < m_minScale) newScale = m_minScale;
    if (newScale > m_maxScale) newScale = m_maxScale;

    // 如果缩放比例未变化则退出
    if (qFuzzyCompare(newScale, m_scaleFactor)) return;

    // 保存缩放前的相对位置
    QPointF mousePos = event->position();
    QPointF imgRelPos((mousePos.x() - m_imagePos.x()) / m_scaleFactor,
                      (mousePos.y() - m_imagePos.y()) / m_scaleFactor);

    // 更新缩放因子
    m_scaleFactor = newScale;

    // 缩放图片（保持原始图片质量）
    m_displayImage = m_originalImage.scaled(
        m_originalImage.size() * m_scaleFactor,
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation
        );

    // 计算新的图片位置（保持鼠标位置相对不变）
    m_imagePos.rx() = mousePos.x() - imgRelPos.x() * m_scaleFactor;
    m_imagePos.ry() = mousePos.y() - imgRelPos.y() * m_scaleFactor;

    // 确保图片位置在合理范围内
    clampImagePosition();

    update(); // 请求重绘
}

void MapWidget::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    if (!m_originalImage.isNull()) {
        // 仅在初始缩放状态下自适应窗口
        if (qFuzzyCompare(m_scaleFactor, m_minScale)) {
            m_displayImage = m_originalImage.scaled(
                size(),
                Qt::KeepAspectRatio,
                Qt::SmoothTransformation
                );
        }

        // 更新初始位置（居中）
        m_initialImagePos = QPointF(
            (width() - m_displayImage.width()) / 2.0,
            (height() - m_displayImage.height()) / 2.0
            );

        // 确保图片位置在合理范围内
        clampImagePosition();

        update();
    }
}

void MapWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && !m_originalImage.isNull()) {
        // 开始拖拽
        m_dragging = true;
        m_dragStartPos = event->pos();
        m_dragStartImagePos = m_imagePos;

        // 改变鼠标光标为抓取状态
        setCursor(Qt::ClosedHandCursor);
    }

    QWidget::mousePressEvent(event);
}

void MapWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (m_dragging) {
        // 计算鼠标移动的偏移量
        QPoint delta = event->pos() - m_dragStartPos;

        // 更新图片位置
        m_imagePos = m_dragStartImagePos + delta;

        // 确保图片位置在合理范围内
        clampImagePosition();

        update(); // 请求重绘
    }

    QWidget::mouseMoveEvent(event);
}

void MapWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && m_dragging) {
        // 结束拖拽
        m_dragging = false;

        // 恢复鼠标光标
        setCursor(Qt::ArrowCursor);
    }
    else if (event->button() == Qt::RightButton) {
        // 右键单击恢复初始状态
        resetToInitialState();
    }

    QWidget::mouseReleaseEvent(event);
}
