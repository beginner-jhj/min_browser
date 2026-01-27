#include "css/layout_tree.h"
#include "util_functions.h"

// ============================================================================
// Image Layout Helper
// ============================================================================

/**
 * \brief Handles layout calculation for image elements.
 * 
 * Loads the image from various sources (local file, URL, or data URI),
 * calculates dimensions based on CSS width/height properties, and positions
 * the image within the layout flow.
 * 
 * \param node The image node
 * \param base_url Base URL for resolving relative image URLs
 * \param image_cache_manager Image cache manager for loading remote images
 * \param line Current line state for positioning
 * \return LAYOUT_BOX containing the image layout
 */
LAYOUT_BOX layout_image_element(
    std::shared_ptr<NODE> node,
    const QString &base_url,
    IMAGE_CACHE_MANAGER *image_cache_manager,
    LINE_STATE &line)
{
    LAYOUT_BOX box;
    box.node = node;
    box.style = node->get_all_styles();

    QString src = QString::fromStdString(node->get_attribute("src"));
    if (src.isEmpty())
        return box;

    QString absolute_url = resolve_url(base_url, src);
    QPixmap image;

    if (absolute_url.startsWith("file://")) {
        QString local_path = QUrl(absolute_url).toLocalFile();
        image.load(local_path);
    }
    else if (absolute_url.startsWith("http://") || absolute_url.startsWith("https://")) {
        auto image_it = image_cache_manager->image_cacher.find(absolute_url);
        if (image_it != image_cache_manager->image_cacher.end() && !image_it->second.isNull()) {
            image = image_it->second;
        }
        else {
            box.width = 0;
            box.height = 0;
            image_cache_manager->image_network_manager->get(QNetworkRequest(src));
            image_cache_manager->src = absolute_url;
            return box;
        }
    }
    else if (absolute_url.startsWith("data:")) {
        int comma_index = absolute_url.indexOf(',');
        if (comma_index != -1) {
            QString base64_encoded = absolute_url.mid(comma_index + 1);
            QByteArray image_data = QByteArray::fromBase64(base64_encoded.toUtf8());
            image.loadFromData(image_data);
        }
    }

    if (!image.isNull()) {
        // Calculate dimensions based on CSS properties
        if (box.style.width < 0 && box.style.height < 0) {
            box.width = image.width();
            box.height = image.height();
        }
        else if (box.style.width > 0 && box.style.height < 0) {
            box.width = box.style.width;
            box.height = box.width * image.height() / image.width();
        }
        else if (box.style.width < 0 && box.style.height > 0) {
            box.height = box.style.height;
            box.width = box.height * image.width() / image.height();
        }
        else {
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

// ============================================================================
// Text Layout Helper
// ============================================================================

/**
 * \brief Handles layout calculation for text nodes.
 * 
 * Splits text into words and calculates their positions on the current line,
 * handling word wrapping when content exceeds available width.
 * 
 * \param root The text node
 * \param style The computed style for the text
 * \param line Current line state for positioning
 * \return LAYOUT_BOX containing the text layout
 */
LAYOUT_BOX layout_text_element(
    std::shared_ptr<NODE> root,
    const COMPUTED_STYLE &style,
    LINE_STATE &line)
{
    LAYOUT_BOX box;
    box.node = root;
    box.style = style;

    QFont font = style.to_font();
    QFontMetrics metrics(font);

    std::string text = root->get_text_content();
    std::vector<std::string> words = split_into_words(text);

    for (auto &word : words) {
        int word_width = metrics.horizontalAdvance(QString::fromStdString(word));
        int word_height = metrics.height();

        bool will_wrap = (line.current_x + word_width > line.max_width) && line.current_x > 0;
        if (will_wrap) {
            line.current_x = line.padding_left;
            line.current_y += line.line_height;
            line.line_height = 0;
        }

        LAYOUT_BOX word_box;
        word_box.node = root;
        word_box.text = word;
        word_box.style = style;
        word_box.x = line.current_x;
        word_box.y = line.current_y;
        word_box.width = word_width;
        word_box.height = word_height;

        box.children.push_back(word_box);
        line.current_x += word_width;

        float effective_line_height = std::max(static_cast<float>(word_height), style.line_height);
        if (effective_line_height > line.line_height) {
            line.line_height = effective_line_height;
        }
    }

    if (!box.children.empty()) {
        box.x = box.children[0].x;
        box.y = box.children[0].y;
        box.width = line.current_x - box.x;
        box.height = line.line_height;
    }

    return box;
}

// ============================================================================
// Block Layout Helper
// ============================================================================

/**
 * \brief Handles layout calculation for block-level elements.
 * 
 * Calculates width, positions children vertically, handles absolute/fixed
 * positioned children separately, and calculates height based on content.
 * 
 * \param root The block element node
 * \param parent_width Available width for layout
 * \param line Current line state
 * \param base_url Base URL for resource resolution
 * \param image_cache_manager Image cache manager
 * \return LAYOUT_BOX containing the block layout
 */
LAYOUT_BOX layout_block_element(
    std::shared_ptr<NODE> root,
    float parent_width,
    LINE_STATE &line,
    const QString &base_url,
    IMAGE_CACHE_MANAGER *image_cache_manager)
{
    LAYOUT_BOX box;
    box.node = root;
    box.style = root->get_all_styles();

    // Calculate width
    if (box.style.width > 0) {
        box.width = box.style.width;
    }
    else {
        box.width = parent_width - box.style.margin_left - box.style.margin_right;
    }

    box.is_positioned = box.style.position != POSITION_TYPE::Static;

    // Initialize line state for block's children
    line.current_x = box.style.padding_left;
    line.current_y = box.style.padding_top;
    line.line_height = 0;
    line.max_width = box.width - box.style.padding_right - box.style.padding_left;
    line.padding_left = box.style.padding_left;

    float content_y = box.style.padding_top;
    float child_parent_width = box.width - box.style.padding_left - box.style.padding_right;

    for (auto child : root->get_children()) {
        LAYOUT_BOX child_box = create_layout_tree(child, child_parent_width, line, base_url, image_cache_manager);

        // Handle positioned children
        if (child_box.style.position == POSITION_TYPE::Absolute) {
            if (child_box.style.width > 0) {
                child_box.width = child_box.style.width;
            } else {
                child_box.width = child_parent_width;
            }
            child_box.x = box.style.padding_left + child_box.style.left;
            child_box.y = box.style.padding_top + child_box.style.top;
            box.absolute_children.push_back(child_box);
            continue;
        }
        else if (child_box.style.position == POSITION_TYPE::Fixed) {
            if (child_box.style.width > 0) {
                child_box.width = child_box.style.width;
            } else {
                child_box.width = child_parent_width;
            }
            child_box.x = 0;
            child_box.y = 0;
            box.absolute_children.push_back(child_box);
            continue;
        }

        // Position child in flow
        if (child_box.style.display == DISPLAY_TYPE::BLOCK) {
            child_box.x = child_box.style.margin_left + box.style.padding_left;
            child_box.y = content_y + child_box.style.margin_top;
            content_y += child_box.height + child_box.style.margin_top + child_box.style.margin_bottom;
            line.current_x = box.style.padding_left;
            line.current_y = content_y;
            line.line_height = 0;
        }
        else {
            float inline_bottom = line.current_y + line.line_height;
            if (inline_bottom > content_y) {
                content_y = inline_bottom;
            }
        }

        box.children.push_back(child_box);
    }

    // Calculate height
    if (box.style.height > 0) {
        box.height = box.style.height;
    } else {
        box.height = content_y + box.style.padding_bottom;
    }

    return box;
}

// ============================================================================
// Inline Layout Helper
// ============================================================================

/**
 * \brief Handles layout calculation for inline elements.
 * 
 * Processes children inline with proper spacing, wrapping across lines,
 * and height calculation considering padding.
 * 
 * \param root The inline element node
 * \param parent_width Available width for layout
 * \param line Current line state
 * \param base_url Base URL for resource resolution
 * \param image_cache_manager Image cache manager
 * \return LAYOUT_BOX containing the inline layout
 */
LAYOUT_BOX layout_inline_element(
    std::shared_ptr<NODE> root,
    float parent_width,
    LINE_STATE &line,
    const QString &base_url,
    IMAGE_CACHE_MANAGER *image_cache_manager)
{
    LAYOUT_BOX box;
    box.node = root;
    box.style = root->get_all_styles();

    // Apply left spacing (margin + padding)
    float left_spacing = box.style.margin_left + box.style.padding_left;
    line.current_x += left_spacing;

    float start_x = line.current_x;
    float start_y = line.current_y;
    float start_line_height = line.line_height;

    // Process children
    for (auto child : root->get_children()) {
        LAYOUT_BOX child_box = create_layout_tree(child, parent_width, line, base_url, image_cache_manager);
        box.children.push_back(child_box);
    }

    float end_x = line.current_x;

    // Apply right spacing (padding + margin)
    float right_spacing = box.style.padding_right + box.style.margin_right;
    line.current_x += right_spacing;

    // Calculate height with padding
    float content_height = line.line_height - start_line_height;
    float total_height = content_height + box.style.padding_top + box.style.padding_bottom;

    // Update line height if this inline element is taller
    if (total_height > line.line_height) {
        line.line_height = total_height;
    }

    // Set box dimensions
    box.y = 0;
    box.x = 0;
    box.width = end_x - start_x;
    box.height = total_height;

    return box;
}

// ============================================================================
// Main Layout Tree Creation
// ============================================================================
// ============================================================================
// Main Layout Tree Creation
// ============================================================================

/**
 * \brief Converts a DOM tree into a layout tree with computed positions and dimensions.
 *
 * Recursively traverses the DOM tree and creates a parallel layout tree (LAYOUT_BOX structures)
 * with calculated positions, dimensions, and visual properties. Dispatches to specialized
 * layout helpers based on element type (image, text, block, inline).
 *
 * \param root The DOM node to layout.
 * \param parent_width The available width for layout in pixels.
 * \param line The current line state tracking horizontal and vertical positions.
 * \param base_url The base URL for resolving relative image and resource URLs.
 * \param image_cache_manager Pointer to the image cache manager for loading images.
 * \return A LAYOUT_BOX representing the complete layout of the subtree.
 */
LAYOUT_BOX create_layout_tree(
    std::shared_ptr<NODE> root,
    float parent_width,
    LINE_STATE &line, const QString &base_url, IMAGE_CACHE_MANAGER *image_cache_manager)
{
    LAYOUT_BOX box;
    box.node = root;
    box.style = root->get_all_styles();

    // Skip display:none elements
    if (box.style.display == DISPLAY_TYPE::NONE) {
        return box;
    }

    // Delegate to specialized handlers based on element type
    if (root->get_tag_name() == "img" && image_cache_manager != nullptr) {
        return layout_image_element(root, base_url, image_cache_manager, line);
    }

    if (root->get_type() == NODE_TYPE::TEXT) {
        std::string text = root->get_text_content();
        text.erase(0, text.find_first_not_of(" \t\n\r"));
        text.erase(text.find_last_not_of(" \t\n\r") + 1);
        if (!text.empty()) {
            return layout_text_element(root, box.style, line);
        }
        return box;
    }

    if (box.style.display == DISPLAY_TYPE::BLOCK) {
        return layout_block_element(root, parent_width, line, base_url, image_cache_manager);
    }

    if (box.style.display == DISPLAY_TYPE::INLINE) {
        return layout_inline_element(root, parent_width, line, base_url, image_cache_manager);
    }

    return box;
}