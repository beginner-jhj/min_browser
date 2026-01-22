#pragma once
#include <QWidget>
#include <QPainter>
#include <QPaintEvent>
#include <memory>
#include "html/node.h"
#include "css/cssom.h"
#include "css/layout_tree.h"
#include "gui/image_cache_manager.h"

class Renderer : public QWidget
{
private:
    std::shared_ptr<Node> m_root;
    CSSOM m_cssom;
    int m_viewport_width, m_viewport_height;

    void paint_layout(QPainter &painter, const LayoutBox &box, float offset_x, float offset_y, const LayoutBox *parent_box = nullptr);
    void paint_fixed(QPainter &painter, const LayoutBox &box);
    void recalculate_layout();
    LayoutBox m_layout_tree;
    bool m_has_layout = false;
    IMAGE_CACHE_MANAGER *m_image_cache_manager;

    QString m_base_url;
protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

public:
    explicit Renderer(QWidget *parent = nullptr);
    void set_document(std::shared_ptr<Node> root, IMAGE_CACHE_MANAGER &image_cache_manager, const QString &base_url="");
};