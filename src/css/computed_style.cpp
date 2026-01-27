#include "css/computed_style.h"
#include "html/node.h"
#include <QDebug>
#include <sstream>

std::unordered_map<std::string, COMPUTED_STYLE::Setter> COMPUTED_STYLE::setters;

static bool initialized = false;

/**
 * \brief Initializes the CSS property setters for COMPUTED_STYLE.
 *
 * Creates and populates the static setters map with lambda functions that handle
 * CSS property assignments. Supports properties including font, color, spacing,
 * borders, display, positioning, and text styling. Each setter parses the CSS value
 * string and applies it to the COMPUTED_STYLE member variable.
 *
 * Only initializes once; subsequent calls are no-ops due to initialization guard.
 */
void COMPUTED_STYLE::init_setters()
{
    if (initialized)
        return;
    initialized = true;

    setters.clear();

    setters["color"] = [](COMPUTED_STYLE &style, const std::string &value)
    {
        style.color = parse_color(value);
    };

    setters["font-size"] = [](COMPUTED_STYLE &style, const std::string &value)
    {
        style.font_size = parse_font_size(value);
    };

    setters["font-weight"] = [](COMPUTED_STYLE &style, const std::string &value)
    {
        try
        {
            if (value == "normal")
            {
                style.font_weight = QFont::Normal;
                return;
            }

            if (value == "bold")
            {
                style.font_weight = QFont::Bold;
                return;
            }
            int weight = std::stoi(value);
            int adjusted_weight;
            if (weight >= 100 && weight < 1000)
            {
                adjusted_weight = std::round(weight / 100) * 100;

                if (adjusted_weight <= 100)
                {
                    style.font_weight = QFont::Thin;
                }

                else if (adjusted_weight == 200)
                {
                    style.font_weight = QFont::ExtraLight;
                }

                else if (adjusted_weight == 300)
                {
                    style.font_weight = QFont::Light;
                }

                else if (adjusted_weight == 400)
                {
                    style.font_weight = QFont::Normal;
                }

                else if (adjusted_weight == 500)
                {
                    style.font_weight = QFont::Medium;
                }

                else if (adjusted_weight == 600)
                {
                    style.font_weight = QFont::DemiBold;
                }

                else if (adjusted_weight == 700)
                {
                    style.font_weight = QFont::Bold;
                }

                else if (adjusted_weight == 800)
                {
                    style.font_weight = QFont::ExtraBold;
                }

                else if (adjusted_weight >= 900)
                {
                    style.font_weight = QFont::Black;
                }
            }
            else
            {
                style.font_weight = QFont::Normal;
            }
        }
        catch (...)
        {
            style.font_weight = QFont::Normal;
        }
    };

    setters["font-style"] = [](COMPUTED_STYLE &style, const std::string &value)
    {
        style.font_style = value;
    };

    setters["font-family"] = [](COMPUTED_STYLE &style, const std::string &value)
    {
        style.font_family = QString::fromStdString(value);
    };

    setters["background-color"] = [](COMPUTED_STYLE &style, const std::string &value)
    {
        style.background_color = parse_color(value);
    };

    setters["width"] = [](COMPUTED_STYLE &style, const std::string &value)
    {
        style.width = parse_string_to_float(value, -1);
    };

    setters["height"] = [](COMPUTED_STYLE &style, const std::string &value)
    {
        style.height = parse_string_to_float(value, -1);
    };

    setters["margin-top"] = [](COMPUTED_STYLE &style, const std::string &value)
    {
        style.margin_top = parse_string_to_float(value, 0);
    };

    setters["margin-bottom"] = [](COMPUTED_STYLE &style, const std::string &value)
    {
        style.margin_bottom = parse_string_to_float(value, 0);
    };

    setters["margin-left"] = [](COMPUTED_STYLE &style, const std::string &value)
    {
        style.margin_left = parse_string_to_float(value, 0);
    };

    setters["margin-right"] = [](COMPUTED_STYLE &style, const std::string &value)
    {
        style.margin_right = parse_string_to_float(value, 0);
    };

    setters["margin"] = [](COMPUTED_STYLE &style, const std::string &value)
    {
        std::vector<std::string> parts;
        std::stringstream ss(value);
        std::string part;

        while (ss >> part)
        {
            parts.push_back(part);
        }

        if (parts.size() == 1)
        {
            // margin: 10px → all sides
            float val = parse_string_to_float(parts[0], 0);
            style.margin_top = val;
            style.margin_right = val;
            style.margin_bottom = val;
            style.margin_left = val;
        }
        else if (parts.size() == 2)
        {
            // margin: 10px 20px → vertical horizontal
            float vertical = parse_string_to_float(parts[0], 0);
            float horizontal = parse_string_to_float(parts[1], 0);
            style.margin_top = vertical;
            style.margin_bottom = vertical;
            style.margin_left = horizontal;
            style.margin_right = horizontal;
        }
        else if (parts.size() == 3)
        {
            // margin: 10px 20px 30px → top horizontal bottom
            float top = parse_string_to_float(parts[0], 0);
            float horizontal = parse_string_to_float(parts[1], 0);
            float bottom = parse_string_to_float(parts[2], 0);
            style.margin_top = top;
            style.margin_right = horizontal;
            style.margin_bottom = bottom;
            style.margin_left = horizontal;
        }
        else if (parts.size() >= 4)
        {
            // margin: 10px 20px 30px 40px → top right bottom left
            style.margin_top = parse_string_to_float(parts[0], 0);
            style.margin_right = parse_string_to_float(parts[1], 0);
            style.margin_bottom = parse_string_to_float(parts[2], 0);
            style.margin_left = parse_string_to_float(parts[3], 0);
        }
    };

    setters["padding-top"] = [](COMPUTED_STYLE &style, const std::string &value)
    {
        style.padding_top = parse_string_to_float(value, 0);
    };

    setters["padding-bottom"] = [](COMPUTED_STYLE &style, const std::string &value)
    {
        style.padding_bottom = parse_string_to_float(value, 0);
    };

    setters["padding-left"] = [](COMPUTED_STYLE &style, const std::string &value)
    {
        style.padding_left = parse_string_to_float(value, 0);
    };

    setters["padding-right"] = [](COMPUTED_STYLE &style, const std::string &value)
    {
        style.padding_right = parse_string_to_float(value, 0);
    };

    setters["padding"] = [](COMPUTED_STYLE &style, const std::string &value)
    {
        // Split by whitespace
        std::vector<std::string> parts;
        std::stringstream ss(value);
        std::string part;

        while (ss >> part) // Splits by whitespace
        {
            parts.push_back(part);
        }

        if (parts.size() == 1)
        {
            // padding: 10px → all sides
            float val = parse_string_to_float(parts[0], 0);
            style.padding_top = val;
            style.padding_right = val;
            style.padding_bottom = val;
            style.padding_left = val;
        }
        else if (parts.size() == 2)
        {
            // padding: 10px 20px → vertical horizontal
            float vertical = parse_string_to_float(parts[0], 0);
            float horizontal = parse_string_to_float(parts[1], 0);
            style.padding_top = vertical;
            style.padding_bottom = vertical;
            style.padding_left = horizontal;
            style.padding_right = horizontal;
        }
        else if (parts.size() == 3)
        {
            // padding: 10px 20px 30px → top horizontal bottom
            float top = parse_string_to_float(parts[0], 0);
            float horizontal = parse_string_to_float(parts[1], 0);
            float bottom = parse_string_to_float(parts[2], 0);
            style.padding_top = top;
            style.padding_right = horizontal;
            style.padding_bottom = bottom;
            style.padding_left = horizontal;
        }
        else if (parts.size() >= 4)
        {
            // padding: 10px 20px 30px 40px → top right bottom left
            style.padding_top = parse_string_to_float(parts[0], 0);
            style.padding_right = parse_string_to_float(parts[1], 0);
            style.padding_bottom = parse_string_to_float(parts[2], 0);
            style.padding_left = parse_string_to_float(parts[3], 0);
        }
    };

    setters["border-width"] = [](COMPUTED_STYLE &style, const std::string &value)
    {
        style.border_width = parse_string_to_float(value, 0);
    };

    setters["border-color"] = [](COMPUTED_STYLE &style, const std::string &value)
    {
        style.border_color = parse_color(value);
    };

    setters["border-style"] = [](COMPUTED_STYLE &style, const std::string &value)
    {
        if (value == "solid")
        {
            style.border_style = Qt::SolidLine;
        }

        else if (value == "dashed")
        {
            style.border_style = Qt::DashLine;
        }

        else if (value == "dotted")
        {
            style.border_style = Qt::DotLine;
        }

        else
        {
            style.border_style = Qt::NoPen;
        }
    };

    setters["border"] = [](COMPUTED_STYLE &style, const std::string &value)
    {
        std::vector<std::string> parts;
        std::stringstream ss(value);
        std::string part;

        while (ss >> part)
        {
            parts.push_back(part);
        }

        // Each part could be width, style, or color
        // We need to detect which is which
        for (const auto &p : parts)
        {
            // Check if it's a number (width)
            if (std::isdigit(p[0]))
            {
                style.border_width = parse_string_to_float(p, 0);
            }
            // Check if it's a style keyword
            else if (p == "solid")
            {
                style.border_style = Qt::SolidLine;
            }
            else if (p == "dashed")
            {
                style.border_style = Qt::DashLine;
            }
            else if (p == "dotted")
            {
                style.border_style = Qt::DotLine;
            }
            // Otherwise it's a color
            else
            {
                style.border_color = parse_color(p);
            }
        }
    };

    setters["display"] = [](COMPUTED_STYLE &style, const std::string &value)
    {
        if (value == "inline")
        {
            style.display = DISPLAY_TYPE::INLINE;
        }

        else if (value == "block")
        {
            style.display = DISPLAY_TYPE::BLOCK;
        }

        else
        {
            style.display = DISPLAY_TYPE::NONE;
        }
    };

    setters["box-sizing"] = [](COMPUTED_STYLE &style, const std::string &value)
    {
        if (value == "border-box")
        {
            style.box_sizing = BOX_SIZING::BorderBox;
        }
        else
        {
            style.box_sizing = BOX_SIZING::ContentBox;
        }
    };

    setters["text-align"] = [](COMPUTED_STYLE &style, const std::string &value)
    {
        if (value == "center")
        {
            style.text_align = TEXT_ALIGN::Center;
        }

        else if (value == "right")
        {
            style.text_align = TEXT_ALIGN::Right;
        }

        else if (value == "justify")
        {
            style.text_align = TEXT_ALIGN::Justify;
        }

        else
        {
            style.text_align = TEXT_ALIGN::Left;
        }
    };

    setters["line-height"] = [](COMPUTED_STYLE &style, const std::string &value)
    {
        style.line_height = parse_string_to_float(value, 16 * 1.5);
    };

    setters["visibility"] = [](COMPUTED_STYLE &style, const std::string &value)
    {
        if (value == "hidden" || value == "collapse")
        {
            style.visibility = false;
        }
        else
        {
            style.visibility = true;
        }
    };

    setters["text-decoration"] = [](COMPUTED_STYLE &style, const std::string &value)
    {
        if (value == "underline")
        {
            style.text_decoration = TEXT_DECORATION::UnderLine;
        }

        else if (value == "line-through")
        {
            style.text_decoration = TEXT_DECORATION::LineThrough;
        }

        else if (value == "overline")
        {
            style.text_decoration = TEXT_DECORATION::OverLine;
        }

        else
        {
            style.text_decoration = TEXT_DECORATION::None;
        }
    };

    setters["opacity"] = [](COMPUTED_STYLE &style, const std::string &value)
    {
        style.opacity = parse_string_to_float(value, 1);
        if (style.opacity < 0.0)
            style.opacity = 0.0;
        if (style.opacity > 1.0)
            style.opacity = 1.0;
    };

    setters["position"] = [](COMPUTED_STYLE &style, const std::string &value)
    {
        if (value == "relative")
        {
            style.position = POSITION_TYPE::Relative;
        }
        else if (value == "absolute")
        {
            style.position = POSITION_TYPE::Absolute;
        }
        else if (value == "fixed")
        {
            style.position = POSITION_TYPE::Fixed;
        }
        else
        {
            style.position = POSITION_TYPE::Static;
        }
    };

    setters["top"] = [](COMPUTED_STYLE &style, const std::string &value)
    {
        style.top = parse_string_to_float(value, 0);
        style.is_top_set = true;
    };

    setters["right"] = [](COMPUTED_STYLE &style, const std::string &value)
    {
        style.right = parse_string_to_float(value, 0);
        style.is_right_set = true;
    };

    setters["bottom"] = [](COMPUTED_STYLE &style, const std::string &value)
    {
        style.bottom = parse_string_to_float(value, 0);
        style.is_bottom_set = true;
    };

    setters["left"] = [](COMPUTED_STYLE &style, const std::string &value)
    {
        style.left = parse_string_to_float(value, 0);
        style.is_left_set = true;
    };
}
/**
 * \brief Parses a CSS color value string into a QColor object.
 *
 * Handles both named colors and RGB/RGBA formats. Named colors are passed directly
 * to QColor for interpretation. RGB format (e.g., "rgb(255,128,0)") is parsed by
 * extracting numeric values and converting to QColor. RGBA format includes an alpha
 * channel (opacity) that is scaled from 0-1 to 0-255.
 *
 * \param color_value The color string to parse (e.g., "red", "rgb(255,0,0)", "rgba(255,0,0,0.5)").
 * \return A QColor object representing the parsed color.
 */QColor COMPUTED_STYLE::parse_color(const std::string &color_value)
{
    if (color_value.size() <= 3)
    {
        return QColor(QString::fromStdString(color_value));
    }
    if (color_value.substr(0, 3) != "rgb")
    {
        return QColor(QString::fromStdString(color_value));
    }

    size_t pos = 0;
    std::vector<double> values;
    std::string value;
    while (pos < color_value.size())
    {
        char current = color_value[pos];

        if (std::isdigit(current) || current == '.')
        {
            value += current;
        }

        else if (current == ',' || current == ')')
        {
            if (!value.empty())
            {
                values.push_back(std::stod(value));
            }
            value.clear();
        }

        ++pos;
    }

    if (values.size() == 4)
    {
        return QColor(values[0], values[1], values[2], values[3] * 255);
    }

    return QColor(values[0], values[1], values[2]);
}

/**
 * \brief Parses a CSS font-size value string into pixel units.
 *
 * Converts font-size values from various CSS units (px, pt, cm, mm, in) and
 * CSS keywords (xx-small through xx-large) into pixel values. Uses standard
 * conversion factors (1in = 96px = 2.54cm). Returns 16px as the default/medium
 * font size if parsing fails.
 *
 * \param value The font-size value string (e.g., "14px", "medium", "1.5cm").
 * \return The font size in pixels as an integer.
 */
int COMPUTED_STYLE::parse_font_size(const std::string &value)
/* 1in = 96px = 2.54cm = 25.4mm
    96/2.54 is about 37.8px
*/
{
    bool is_keyword = value.find('-') != std::string::npos;

    if (is_keyword)
    {
        if (value == "xx-small")
        {
            return 9;
        }

        else if (value == "x-small")
        {
            return 10;
        }

        else if (value == "small")
        {
            return 13;
        }

        else if (value == "medium")
        {
            return 16;
        }

        else if (value == "large")
        {
            return 18;
        }

        else if (value == "x-large")
        {
            return 24;
        }

        else if (value == "xx-large")
        {
            return 32;
        }

        else
        {
            return 16;
        }
    }
    else
    {
        std::string unit = value.substr(value.size() - 2);
        try
        {
            double num_value = std::stod(value.substr(0, value.size() - 2));

            if (unit == "px" || unit == "pt")
            {
                return std::round(num_value);
            }

            else if (unit == "cm")
            {
                return std::round(num_value * 37.8);
            }

            else if (unit == "mm")
            {
                return std::round(num_value * 3.78);
            }

            else if (unit == "in")
            {
                return std::round(num_value * 96);
            }

            else
            {
                return 16;
            }
        }
        catch (...)
        {
            return 16;
        }
    }
}

/**
 * \brief Parses a CSS numeric value string with units into a float.
 *
 * Converts string representations of numbers (with or without units) into
 * floating-point values. Handles invalid input gracefully by returning the
 * provided default value.
 *
 * \param value The numeric string to parse (e.g., "10px", "15.5").
 * \param default_value The value to return if parsing fails.
 * \return The parsed float value, or default_value on error.
 */
float COMPUTED_STYLE::parse_string_to_float(const std::string &value, const float default_value)
{
    try
    {
        float num_value = std::stof(value);
        return num_value;
    }
    catch (...)
    {
        return default_value;
    }
}

/**
 * \brief Returns the inherited color value as a hex string.
 *
 * Converts the QColor member to its hexadecimal string representation for
 * use in CSS inheritance to child elements.
 *
 * \return The color as a hex string (e.g., "#ff0000").
 */
std::string COMPUTED_STYLE::inherit_color() const
{
    QString color_hex_name = color.name();
    return color_hex_name.toStdString();
}

/**
 * \brief Returns the inherited font-size value as a string.
 *
 * Converts the font_size member (in pixels) to its string representation
 * for inheritance to child elements.
 *
 * \return The font size as a string (e.g., "14").
 */
std::string COMPUTED_STYLE::inherit_font_size() const
{
    return std::to_string(font_size);
}

/**
 * \brief Returns the inherited font-weight value as a string.
 *
 * Converts the font_weight member (QFont::Weight enum) to its numeric
 * CSS representation for inheritance to child elements.
 *
 * \return The font weight as a string (e.g., "700" for bold, "400" for normal).
 */
std::string COMPUTED_STYLE::inherit_font_weight() const
{
    int weight = font_weight;
    if (weight >= 100)
    {
        return std::to_string(weight);
    }

    else
    {
        if (weight < 10)
        {
            return std::to_string(weight * 100);
        }

        return std::to_string(weight * 10);
    }
}

/**
 * \brief Returns the inherited font-style value as a string.
 *
 * Returns the font_style member for inheritance to child elements.
 *
 * \return The font style as a string (e.g., "italic", "normal").
 */
std::string COMPUTED_STYLE::inherit_font_style() const
{
    return font_style;
}

/**
 * \brief Returns the inherited font-family value as a string.
 *
 * Converts the font_family member (QString) to std::string for inheritance
 * to child elements.
 *
 * \return The font family as a string.
 */
std::string COMPUTED_STYLE::inherit_font_family() const
{
    return font_family.toStdString();
}

/**
 * \brief Returns the inherited line-height value as a string.
 *
 * Returns the line_height member for inheritance to child elements.
 *
 * \return The line height as a string.
 */
std::string COMPUTED_STYLE::inherit_line_height() const
{
    return std::to_string(line_height);
}

/**
 * \brief Returns the inherited text-align value as a string.
 *
 * Converts the text_align member (TextAlign enum) to its CSS string
 * representation for inheritance to child elements.
 *
 * \return The text alignment as a string (e.g., "left", "center", "right", "justify").
 */
std::string COMPUTED_STYLE::inherit_text_align() const
{
    if (text_align == TEXT_ALIGN::Center)
    {
        return "center";
    }

    else if (text_align == TEXT_ALIGN::Justify)
    {
        return "justify";
    }

    else if (text_align == TEXT_ALIGN::Right)
    {
        return "right";
    }

    else
    {
        return "left";
    }
}

/**
 * \brief Returns the inherited visibility value as a string.
 *
 * Converts the visibility boolean member to its CSS string representation
 * for inheritance to child elements.
 *
 * \return The visibility as a string ("visible" or "hidden").
 */
std::string COMPUTED_STYLE::inherit_visibility() const
{
    if (!visibility)
    {
        return "hidden";
    }
    else
    {
        return "visible";
    }
}

/**
 * \brief Returns the inherited text-decoration value as a string.
 *
 * Converts the text_decoration member (TextDecoration enum) to its CSS string
 * representation for inheritance to child elements.
 *
 * \return The text decoration as a string (e.g., "none", "underline", "line-through", "overline").
 */
std::string COMPUTED_STYLE::inherit_text_decoration() const
{
    switch (text_decoration)
    {
    case TEXT_DECORATION::UnderLine:
        return "underline";
    case TEXT_DECORATION::LineThrough:
        return "line-through";
    case TEXT_DECORATION::OverLine:
        return "overline";
    default:
        return "none";
    }
}
