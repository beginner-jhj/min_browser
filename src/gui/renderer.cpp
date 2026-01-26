#include "gui/renderer.h"
#include "css/css_parser.h"
#include "css/apply_style.h"
#include <QScrollArea>
#include <QScrollBar>
#include <QResizeEvent>
#include <queue>
#include "util_functions.h"

Renderer::Renderer(QWidget *parent) : QWidget(parent), m_root(nullptr), m_viewport_height(0), m_viewport_width(0), m_image_cache_manager(nullptr), m_current_history_it(m_history_list.begin())
{
}

float calculate_content_width(const LayoutBox &box);

void Renderer::set_document(std::shared_ptr<Node> root, IMAGE_CACHE_MANAGER &image_cache_manager, const QString &base_url)
{
    m_image_cache_manager = &image_cache_manager;

    if (m_history_list.size() > 0)
    {
        auto next_it = std::next(m_current_history_it);
        if (next_it != m_history_list.end())
        {
            m_history_list.erase(next_it, m_history_list.end());
        }
    }

    m_history_list.push_back({root, base_url});

    m_current_history_it = std::prev(m_history_list.end());

    render(root, base_url);
}

void Renderer::render(std::shared_ptr<Node> root, const QString &base_url)
{
    m_root = root;
    m_base_url = base_url;

    if (m_root)
    {
        std::string user_agent_css = R"(
    /* Hidden elements */
    head, style, script, meta, link, title { display: none; }
    
    /* Block elements */
    html, body, div, p, h1, h2, h3, h4, h5, h6, ul, ol, li, footer, header, section { 
        display: block; 
    }
    
    /* Default spacing for the page */
    body { 
        padding: 8px; 
        margin: 0;
        line-height: 1.2;
    }

    /* Heading Styles */
    h1 { font-size: 32px; font-weight: bold; margin-top: 21px; margin-bottom: 21px; }
    h2 { font-size: 24px; font-weight: bold; margin-top: 19px; margin-bottom: 19px; }
    h3 { font-size: 18px; font-weight: bold; margin-top: 18px; margin-bottom: 18px; }
    h4 { font-size: 16px; font-weight: bold; margin-top: 21px; margin-bottom: 21px; }
    h5 { font-size: 13px; font-weight: bold; margin-top: 22px; margin-bottom: 22px; }
    h6 { font-size: 10px; font-weight: bold; margin-top: 24px; margin-bottom: 24px; }

    /* Paragraph & List Spacing */
    p { margin-top: 16px; margin-bottom: 16px; }
    ul, ol { padding-left: 40px; margin-top: 16px; margin-bottom: 16px; }

    /* Inline elements */
    span, a, strong, em { display: inline; }
    strong { font-weight: bold; }
    em { font-style: italic; }
    a { color: blue; text-decoration: underline; }
)";
        std::string author_css = extract_stylesheets(m_root);

        std::string combined_css = user_agent_css + "\n" + author_css;
        m_cssom = create_cssom(combined_css);

        apply_style(m_root, m_cssom);

        recalculate_layout();
    }
    update();
}

void Renderer::resizeEvent(QResizeEvent *event)
{
    if (m_root && event->oldSize().width() != event->size().width())
    {
        recalculate_layout();
    }

    QWidget::resizeEvent(event);
}

void Renderer::recalculate_layout()
{
    if (!m_root)
        return;

    int current_width = this->width();
    if (current_width <= 0)
    {
        QWidget *scroll_area = parentWidget(); // Viewport
        if (scroll_area)
            current_width = scroll_area->width();
        if (current_width <= 0)
            current_width = 1000;
    }

    LineState line(current_width);

    m_layout_tree = create_layout_tree(m_root, current_width, line, m_base_url, m_image_cache_manager);
    m_has_layout = true;

    float content_width = calculate_content_width(m_layout_tree);
    float final_height = m_layout_tree.height;

    float min_width = std::max(static_cast<float>(current_width), content_width);

    this->setMinimumSize(min_width, final_height);
}
void Renderer::paintEvent(QPaintEvent *event)
{
    if (!m_root)
    {
        return;
    }

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.fillRect(rect(), Qt::white);

    paint_layout(painter, m_layout_tree, 0, 0);
}

void Renderer::paint_layout(QPainter &painter, const LayoutBox &box, float offset_x, float offset_y, const LayoutBox *parent_box)
{
    float abs_x = offset_x + box.x;
    float abs_y = offset_y + box.y;

    float previous_opacity = painter.opacity();
    painter.setOpacity(previous_opacity * box.style.opacity);

    if (box.style.position == PositionType::Relative)
    {
        abs_x += box.style.left - box.style.right;
        abs_y += box.style.top - box.style.bottom;
    }

    if (box.node->get_type() == NODE_TYPE::ELEMENT)
    {
        if (!box.image.isNull())
        {
            painter.drawPixmap(abs_x, abs_y, box.width, box.height, box.image);
            return;
        }
        if (box.style.background_color != QColor("transparent"))
        {
            painter.fillRect(
                abs_x, abs_y,
                box.width, box.height,
                box.style.background_color);
        }

        if (box.style.border_width > 0)
        {
            QPen pen;
            pen.setColor(box.style.border_color);
            pen.setStyle(box.style.border_style);
            pen.setWidthF(box.style.border_width);

            painter.setPen(pen);
            painter.setBrush(Qt::NoBrush);

            painter.drawRect(abs_x, abs_y, box.width, box.height);
        }
    }

    if (box.node->get_type() == NODE_TYPE::TEXT)
    {
        QFont ft = box.style.to_font();
        painter.setFont(ft);
        painter.setPen(box.style.color);

        QFontMetrics metrics(ft);

        float offset_adjust = 0;

        if (parent_box)
        {
            float total_width = 0;
            for (const auto &word_box : box.children)
            {
                total_width += word_box.width;
            }

            if (parent_box->style.text_align == TextAlign::Center)
            {
                offset_adjust = (parent_box->width - total_width) / 2;
            }

            else if (parent_box->style.text_align == TextAlign::Right)
            {
                offset_adjust = parent_box->width - total_width;
            }
        }

        if (parent_box && parent_box->node->get_tag_name() == "li")
        {
            painter.drawText(offset_x + box.x + offset_adjust, offset_y + box.y + metrics.ascent(), "•");
            offset_adjust += 15;
        }

        for (const auto &word_box : box.children)
        {
            float word_abs_x = offset_x + word_box.x + offset_adjust;
            float word_abs_y = offset_y + word_box.y;
            float baseline_y = word_abs_y + metrics.ascent();

            painter.drawText(word_abs_x, baseline_y, QString::fromStdString(word_box.text));

            if (word_box.style.text_decoration != TextDecoration::None)
            {
                QPen decoration_pen(box.style.color);

                decoration_pen.setWidth(1);
                painter.setPen(decoration_pen);

                float decoration_y = 0;

                switch (box.style.text_decoration)
                {
                case TextDecoration::UnderLine:
                    decoration_y = baseline_y + 1;
                    break;

                case TextDecoration::LineThrough:
                    decoration_y = word_abs_y + metrics.ascent() / 2;
                    break;

                default:
                    decoration_y = word_abs_y;
                    break;
                }

                painter.drawLine(word_abs_x, decoration_y, word_abs_x + word_box.width, decoration_y);
            }
        }

        painter.setPen(box.style.color);

        return;
    }

    for (const auto &child : box.children)
    {
        paint_layout(painter, child, abs_x, abs_y, &box);
    }

    for (const auto &abs_child : box.absolute_children)
    {
        if (abs_child.style.position == PositionType::Fixed)
        {
            paint_fixed(painter, abs_child);
        }

        else
        {
            paint_layout(painter, abs_child, abs_x, abs_y, &box);
        }
    }
}

void Renderer::paint_fixed(QPainter &painter, const LayoutBox &box)
{
    // QScrollArea -> ViewPort(hidden) -> renderer
    QScrollArea *scroll_area = qobject_cast<QScrollArea *>(parentWidget()->parentWidget());
    int scroll_x = 0;
    int scroll_y = 0;

    if (scroll_area)
    {
        scroll_x = scroll_area->horizontalScrollBar()->value();
        scroll_y = scroll_area->verticalScrollBar()->value();
    }

    float draw_x = 0, draw_y = 0;

    if (box.style.is_left_set)
    {
        draw_x = scroll_x + box.style.left;
    }

    else if (box.style.is_right_set)
    {
        draw_x = scroll_x + m_viewport_width - box.width - box.style.right;
    }
    else
    {
        draw_x = scroll_x;
    }

    if (box.style.is_top_set)
    {
        draw_y = scroll_y + box.style.top;
    }

    else if (box.style.is_bottom_set)
    {
        draw_y = scroll_y + m_viewport_height - box.height - box.style.top;
    }
    else
    {
        draw_y = scroll_y;
    }

    float previous_opacity = painter.opacity();
    painter.setOpacity(previous_opacity * box.style.opacity);

    if (box.style.background_color != QColor("transparent"))
    {
        painter.fillRect(draw_x, draw_y, box.width, box.height, box.style.background_color);
    }

    if (box.style.border_width > 0)
    {
        QPen pen;
        pen.setColor(box.style.border_color);
        pen.setStyle(box.style.border_style);
        pen.setWidthF(box.style.border_width);

        painter.setPen(pen);
        painter.setBrush(Qt::NoBrush);

        painter.drawRect(draw_x, draw_y, box.width, box.height);
    }

    for (const auto &child : box.children)
    {
        paint_layout(painter, child, draw_x, draw_y, &box);
    }

    painter.setOpacity(previous_opacity);
}

float calculate_content_width(const LayoutBox &root)
{
    float max_right = root.x + root.width;

    std::queue<std::pair<LayoutBox, float>> q;
    q.push({root, 0.0f});

    while (!q.empty())
    {
        auto [current_box, parent_abs_x] = q.front();
        q.pop();

        float current_abs_x = parent_abs_x + current_box.x;
        float current_right = current_abs_x + current_box.width;

        if (current_right > max_right)
        {
            max_right = current_right;
        }

        for (const auto &child : current_box.children)
        {
            q.push({child, current_abs_x});
        }
    }

    return max_right - root.x;
}

void Renderer::mousePressEvent(QMouseEvent *event)
{
    float x = event->pos().x();
    float y = event->pos().y();

    auto clicked_node = find_node_at(x, y);
    if (clicked_node)
    {
        std::string href = bubble_for_link(clicked_node);

        if (!href.empty())
        {
            QString absolute_url = resolve_url(m_base_url, QString::fromStdString(href));
            emit link_clicked(absolute_url);
        }
    }
}

std::shared_ptr<Node> Renderer::find_node_at(float x, float y)
{
    return find_node_in_box(m_layout_tree, x, y, 0, 0);
}

std::shared_ptr<Node> Renderer::find_node_in_box(const LayoutBox &box, float x, float y, float offset_x, float offset_y)
{
    float abs_x = offset_x + box.x;
    float abs_y = offset_y + box.y;

    if (box.style.position == PositionType::Relative)
    {
        abs_x += box.style.left - box.style.right;
        abs_y += box.style.top - box.style.bottom;
    }

    // FIRST: Always check children, regardless of this box's bounds
    // Why: For inline elements, parent box might be (0,0) but children have real positions
    for (const auto &child : box.children)
    {
        auto result = find_node_in_box(child, x, y, abs_x, abs_y);
        if (result)
        {
            return result;
        }
    }

    for (const auto &abs_child : box.absolute_children)
    {
        if (abs_child.style.position == PositionType::Fixed)
        {
            continue;
        }
        auto result = find_node_in_box(abs_child, x, y, abs_x, abs_y);
        if (result)
        {
            return result;
        }
    }

    // THEN: Check if click is inside this box's bounds
    bool inside = (x >= abs_x && x <= abs_x + box.width &&
                   y >= abs_y && y <= abs_y + box.height);

    if (!inside)
    {
        return nullptr;
    }

    // If no children matched and we're inside, return this node
    if (box.node)
    {
        return box.node;
    }

    return nullptr;
}

std::string Renderer::bubble_for_link(std::shared_ptr<Node> node)
{
    std::shared_ptr<Node> current = node;

    while (current)
    {
        if (current->get_tag_name() == "a")
        {
            std::string href = current->get_attribute("href");
            if (!href.empty())
            {
                return href;
            }
        }

        current = current->get_parent();
    }

    return "";
}

void Renderer::go_back()
{
    if (m_current_history_it != m_history_list.begin())
    {
        m_current_history_it = std::prev(m_current_history_it);
        PAGE current_page = *m_current_history_it;
        render(current_page.page_root, current_page.base_url);
    }
}

void Renderer::go_forward()
{
    auto next_it = std::next(m_current_history_it);
    if (next_it != m_history_list.end())
    {
        m_current_history_it = next_it;
        PAGE current_page = *m_current_history_it;
        render(current_page.page_root, current_page.base_url);
    }
}
