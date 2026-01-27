#pragma once
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <QRectF>
#include "css/computed_style.h"

enum class NODE_TYPE{
    ELEMENT,TEXT
};

class NODE: public std::enable_shared_from_this<NODE>{
    private:
        NODE_TYPE m_type;
        std::string m_tag_name;
        std::string m_text;

        std::vector<std::shared_ptr<NODE>> m_children;
        std::map<std::string, std::string> m_attributes;
        COMPUTED_STYLE m_computed_style;

        std::weak_ptr<NODE> m_parent;


    public:
        NODE(NODE_TYPE t, const std::string& content);

        void add_child(std::shared_ptr<NODE> child);
        void set_attribute(const std::string& name, const std::string& value);
        std::string get_attribute(const std::string& name) const;

        void set_parent(std::weak_ptr<NODE> parent);

        void set_style(const std::string& name, const std::string& value);
        COMPUTED_STYLE get_all_styles() const;

        const std::string get_tag_name() const;
        const std::string get_text_content() const;
        const NODE_TYPE get_type() const;
        const DISPLAY_TYPE get_display_type() const;

        const std::vector<std::shared_ptr<NODE>>& get_children() const;
        std::shared_ptr<NODE> get_parent() const ;
};
