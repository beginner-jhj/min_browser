#include <QColor>
#include <QFont>
#include <QFontMetrics>
#include <functional>
#include <unordered_map>

enum class BoxSizing
{
    ContentBox,
    BorderBox
};
enum class TextAlign
{
    Left,
    Center,
    Right,
    Justify
};

struct ComputedStyle
{
    QColor color = QColor("#000000");          // default: black
    int font_size = 16;                        // default: 16px
    QFont::Weight font_weight = QFont::Normal; // default: normal
    bool italic = false;
    bool underline = false;
    QString font_family = "Arial";

    QColor background_color = QColor("transparent");
    float width = -1.0;  // -1 = auto
    float height = -1.0; // -1 = auto

    float margin_top = 0.0;
    float margin_right = 0.0;
    float margin_bottom = 0.0;
    float margin_left = 0.0;

    float padding_top = 0.0;
    float padding_right = 0.0;
    float padding_bottom = 0.0;
    float padding_left = 0.0;

    float border_width = 0.0;
    QColor border_color = QColor("#000000");
    Qt::PenStyle border_style = Qt::SolidLine;

    DISPLAY_TYPE display = DISPLAY_TYPE::INLINE;
    BoxSizing box_sizing = BoxSizing::ContentBox;
    TextAlign text_align = TextAlign::Left;

    float line_height = font_size * 1.5;

    bool visibility = true;

    QFont to_font() const
    {
        QFont font;
        font.setPixelSize(font_size);
        font.setWeight(font_weight);
        font.setItalic(italic);
        font.setUnderline(underline);
        font.setFamily(font_family);
        return font;
    }
    QFontMetrics font_metrics() const
    {
        return QFontMetrics(to_font());
    }

    using Setter = std::function<void(ComputedStyle &, const std::string &)>;
    static std::unordered_map<std::string, Setter> setters;

    static QColor parse_color(const std::string &color_value);
    static int parse_font_size(const std::string &value);
    static float parse_string_to_float(const std::string &value, const float default_value = 0);

    static void init_setters();

    ComputedStyle() { init_setters(); };
};