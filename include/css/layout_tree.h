#pragma once
#include <memory>
#include <vector>
#include <QFont>
#include <QFontMetrics>
#include "html/node.h"
#include "css/computed_style.h"
#include <QPixmap>
#include "gui/image_cache_manager.h"

struct LINE_STATE
{
    float current_x = 0;
    float current_y = 0;
    float line_height = 0;
    float max_width = 0;
    float padding_left = 0;

    LINE_STATE(float width = 0) : max_width(width) {}
};

struct LAYOUT_BOX
{
    std::shared_ptr<NODE> node;
    COMPUTED_STYLE style;

    float x = 0;
    float y = 0;
    float width = 0;
    float height = 0;

    std::vector<LAYOUT_BOX> children;
    std::string text;

    bool is_positioned = false;
    std::vector<LAYOUT_BOX> absolute_children;
    QPixmap image;
};

LAYOUT_BOX create_layout_tree(
    std::shared_ptr<NODE> root,
    float parent_width,
    LINE_STATE &line,
    const QString &base_url,
    IMAGE_CACHE_MANAGER *image_cache_manager);