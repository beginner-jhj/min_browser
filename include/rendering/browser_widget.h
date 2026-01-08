#pragma once
#include <QWidget>
#include <QPainter>
#include <QPaintEvent>
#include <memory>
#include "html/node.h"
#include "css/cssom.h"
#include "css/layout_tree.h"

class BrowserWidget : public QWidget
{
private:
    std::shared_ptr<Node> m_root;
    CSSOM m_cssom;
    float m_layout_width;
    float m_layout_height;

    void paint_layout(QPainter& painter, const LayoutBox& box, float offset_x, float offset_y, const LayoutBox *parent_box=nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;

public:
    explicit BrowserWidget(QWidget *parent = nullptr);
    void set_document(std::shared_ptr<Node> root);
    float get_layout_width() const {return m_layout_width;};
    float get_layout_height() const {return m_layout_height;};
};