#pragma once
#include <QWidget>
#include <QPainter>
#include <QPaintEvent>
#include <memory>
#include "html/node.h"
#include "css/cssom.h"
#include "css/layout_tree.h"
#include "gui/image_cache_manager.h"

struct PAGE
{
    std::shared_ptr<Node> page_root;
    QString base_url;
};


class Renderer : public QWidget
{
    Q_OBJECT
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

    std::shared_ptr<Node> find_node_at(float x, float y);
    std::shared_ptr<Node> find_node_in_box(const LayoutBox& box, float x, float y, float offset_x, float offset_y);
    std::string bubble_for_link(std::shared_ptr<Node> node);

    std::list<PAGE> m_history_list;
    std::list<PAGE>::iterator m_current_history_it;

    void render(std::shared_ptr<Node> root, const QString &base_url);

signals:
    void link_clicked(const QString &link);

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;


public slots:
    void go_back();
    void go_forward();

public:
    explicit Renderer(QWidget *parent = nullptr);
    void set_document(std::shared_ptr<Node> root, IMAGE_CACHE_MANAGER &image_cache_manager, const QString &base_url = "");
};