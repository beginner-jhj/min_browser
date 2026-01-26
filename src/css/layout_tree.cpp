#include "css/layout_tree.h"
#include "util_functions.h"

LayoutBox create_layout_tree(
    std::shared_ptr<Node> root,
    float parent_width,
    LineState &line, const QString &base_url, IMAGE_CACHE_MANAGER *image_cache_manager)
{
    LayoutBox box;
    box.node = root;
    box.style = root->get_all_styles();

    if (box.style.display == DISPLAY_TYPE::NONE)
    {
        return box;
    }

    if (box.node->get_tag_name() == "img" && image_cache_manager != nullptr)
    {
        QString src = QString::fromStdString(box.node->get_attribute("src"));
        if (src.isEmpty())
            return box;

        QString absolute_url = resolve_url(base_url, src);

        QPixmap image;

        if (absolute_url.startsWith("file://"))
        {
            QString local_path = QUrl(absolute_url).toLocalFile();
            image.load(local_path);
        }

        else if (absolute_url.startsWith("http://") || absolute_url.startsWith("https://"))
        {
            auto image_it = image_cache_manager->image_cacher.find(absolute_url);
            if (image_it != image_cache_manager->image_cacher.end() && !image_it->second.isNull())
            {
                image = image_it->second;
            }
            else
            {
                box.width = 0;
                box.height = 0;
                image_cache_manager->image_network_manager->get(QNetworkRequest(src));
                image_cache_manager->src = absolute_url;
                return box;
            }
        }

        else if (absolute_url.startsWith("data:"))
        {
            int comma_index = absolute_url.indexOf(',');

            if (comma_index != -1)
            {
                QString base64_encoded = absolute_url.mid(comma_index + 1);
                QByteArray image_data = QByteArray::fromBase64(base64_encoded.toUtf8());

                image.loadFromData(image_data);
            }
        }

        if (!image.isNull())
        {
            if (box.style.width < 0 && box.style.height < 0)
            {
                box.width = image.width();
                box.height = image.height();
            }

            else if (box.style.width > 0 && box.style.height < 0)
            {
                box.width = box.style.width;
                box.height = box.width * image.height() / image.width();
            }

            else if (box.style.width < 0 && box.style.height > 0)
            {
                box.height = box.style.height;
                box.width = box.height * image.width() / image.height();
            }

            else
            {
                box.width = box.style.width;
                box.height = box.style.height;
            }

            box.image = image;

            box.x = box.style.margin_left;
            box.y = line.current_y + box.style.margin_top;

            line.current_y = box.y + box.height + box.style.margin_bottom;
        }

        return box;
    }

    if (root->get_type() == NODE_TYPE::TEXT)
    {
        std::string text = root->get_text_content();
        text.erase(0, text.find_first_not_of(" \t\n\r"));
        text.erase(text.find_last_not_of(" \t\n\r") + 1);
        if (text.empty())
        {
            return box;
        }
    }

    if (box.style.display == DISPLAY_TYPE::BLOCK)
    {
        if (box.style.width > 0)
        {
            box.width = box.style.width;
        }
        else
        {
            box.width = parent_width - box.style.margin_left - box.style.margin_right;
        }

        box.is_positioned = box.style.position != PositionType::Static;

        line.current_x = box.style.padding_left;
        line.current_y = box.style.padding_top;
        line.line_height = 0;
        line.max_width = box.width - box.style.padding_right - box.style.padding_left;
        line.padding_left = box.style.padding_left;

        float content_y = box.style.padding_top;
        for (auto child : root->get_children())
        {

            float child_parent_width = box.width -
                                       box.style.padding_left -
                                       box.style.padding_right;
            LayoutBox child_box = create_layout_tree(child, child_parent_width, line, base_url, image_cache_manager);

            if (child_box.style.position == PositionType::Absolute)
            {
                if (child_box.style.width > 0)
                {
                    child_box.width = child_box.style.width;
                }

                else
                {
                    child_box.width = child_parent_width;
                }
                child_box.x = box.style.padding_left + child_box.style.left;
                child_box.y = box.style.padding_top + child_box.style.top;

                box.absolute_children.push_back(child_box);
                continue;
            }

            else if (child_box.style.position == PositionType::Fixed)
            {
                if (child_box.style.width > 0)
                {
                    child_box.width = child_box.style.width;
                }

                else
                {
                    child_box.width = child_parent_width;
                }

                child_box.x = 0;
                child_box.y = 0;
                box.absolute_children.push_back(child_box);
                continue;
            }

            if (child_box.style.display == DISPLAY_TYPE::BLOCK)
            {
                child_box.x = child_box.style.margin_left + box.style.padding_left;
                child_box.y = content_y + child_box.style.margin_top;
                content_y += child_box.height + child_box.style.margin_top + child_box.style.margin_bottom;

                line.current_x = box.style.padding_left;
                line.current_y = content_y;
                line.line_height = 0;
            }
            else
            {
                float inline_bottom = line.current_y + line.line_height;
                if (inline_bottom > content_y)
                {
                    content_y = inline_bottom;
                }
            }

            box.children.push_back(child_box);
        }

        if (box.style.height > 0)
        {
            box.height = box.style.height;
        }
        else
        {
            box.height = content_y + box.style.padding_bottom;
        }

        return box;
    }

    if (root->get_type() == NODE_TYPE::TEXT)
    {
        QFont font = box.style.to_font();
        QFontMetrics metrics(font);

        std::string text = root->get_text_content();
        std::vector<std::string> words = split_into_words(text);

        for (auto &word : words)
        {
            int word_width = metrics.horizontalAdvance(QString::fromStdString(word));
            int word_height = metrics.height();

            bool will_wrap = (line.current_x + word_width > line.max_width) && line.current_x > 0;

            if (will_wrap)
            {
                line.current_x = line.padding_left;
                line.current_y += line.line_height;
                line.line_height = 0;
            }

            LayoutBox word_box;
            word_box.node = root;
            word_box.text = word;
            word_box.style = box.style;
            word_box.x = line.current_x;
            word_box.y = line.current_y;
            word_box.width = word_width;
            word_box.height = word_height;

            box.children.push_back(word_box);

            line.current_x += word_width;

            float effective_line_height = std::max(static_cast<float>(word_height), box.style.line_height);
            if (effective_line_height > line.line_height)
            {
                line.line_height = effective_line_height;
            }
        }

        box.x = box.children[0].x;
        box.y = box.children[0].y;
        box.width = line.current_x - box.x;
        box.height = line.line_height;

        return box;
    }

    if (box.style.display == DISPLAY_TYPE::INLINE)
    {
        // Step 1: Apply left spacing (margin + padding)
        // Why: Create space before inline element content
        float left_spacing = box.style.margin_left + box.style.padding_left;
        line.current_x += left_spacing;

        float start_x = line.current_x;
        float start_y = line.current_y;
        float start_line_height = line.line_height;

        // Step 2: Process children (text, nested inlines, etc.)
        for (auto child : root->get_children())
        {
            LayoutBox child_box = create_layout_tree(child, parent_width, line, base_url, image_cache_manager);
            box.children.push_back(child_box);
        }

        float end_x = line.current_x;

        // Step 3: Apply right spacing (padding + margin)
        // Why: Create space after inline element content
        float right_spacing = box.style.padding_right + box.style.margin_right;
        line.current_x += right_spacing;

        // Step 4: Calculate height with padding
        // Why: Vertical padding affects line height (margin doesn't)
        float content_height = line.line_height - start_line_height;
        float total_height = content_height + box.style.padding_top + box.style.padding_bottom;

        // Update line height if this inline element is taller
        if (total_height > line.line_height)
        {
            line.line_height = total_height;
        }

        // Step 5: Set box dimensions
        // Position (0,0) because actual position is in children
        // Why: Inline elements can span multiple lines
        box.y = 0;
        box.x = 0;
        box.width = end_x - start_x; // Content width only (no spacing)
        box.height = total_height;

        return box;
    }

    return box;
}