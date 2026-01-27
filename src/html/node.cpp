#include "html/node.h"

/**
 * \brief Constructs a DOM Node with type and content.
 *
 * Creates either an ELEMENT node with a tag name or a TEXT node with text content.
 * The constructor determines the node type and stores the appropriate value.
 *
 * \param t The node type (ELEMENT or TEXT).
 * \param content The tag name (if ELEMENT) or text content (if TEXT).
 */
NODE::NODE(NODE_TYPE t, const std::string &content) : m_type(t)
{
    if (t == NODE_TYPE::ELEMENT)
    {
        m_tag_name = content;
        m_text = "";
    }
    else
    {
        m_text = content;
        m_tag_name = "";
    }
}

/**
 * \brief Adds a child node and establishes parent-child relationship.
 *
 * Appends a child node to this node's children list and sets this node
 * as the child's parent, establishing bidirectional DOM tree relationships.
 *
 * \param child The child Node to add.
 */
void NODE::add_child(std::shared_ptr<NODE> child)
{
    m_children.push_back(child);

    child->set_parent(shared_from_this());
}

/**
 * \brief Returns the tag name of an ELEMENT node.
 *
 * \return The tag name string (e.g., "div", "p", "a"). Empty string for TEXT nodes.
 */
const std::string NODE::get_tag_name() const
{
    return m_tag_name;
}

/**
 * \brief Returns the text content of a TEXT node.
 *
 * \return The text string for TEXT nodes. Empty string for ELEMENT nodes.
 */
const std::string NODE::get_text_content() const
{
    return m_text;
}

/**
 * \brief Returns the type of this node.
 *
 * \return The NODE_TYPE (ELEMENT or TEXT).
 */
const NODE_TYPE NODE::get_type() const
{
    return m_type;
}

/**
 * \brief Returns the default display type for this node based on tag name.
 *
 * Uses the HTML tag name to determine default CSS display property.
 * TEXT nodes are always INLINE. Common block elements like div, p, h1-h6
 * return BLOCK. Other elements return INLINE.
 *
 * \return The DISPLAY_TYPE (BLOCK, INLINE, or NONE).
 */
const DISPLAY_TYPE NODE::get_display_type() const
{
    if (m_type == NODE_TYPE::TEXT)
    {
        return DISPLAY_TYPE::INLINE;
    }

    if (m_tag_name == "div" ||
        m_tag_name == "p" ||
        m_tag_name == "h1" ||
        m_tag_name == "h2" ||
        m_tag_name == "ul" ||
        m_tag_name == "li" ||
        m_tag_name == "ul" ||
        m_tag_name == "ol" ||
        m_tag_name == "li")
    {
        return DISPLAY_TYPE::BLOCK;
    }

    return DISPLAY_TYPE::INLINE;
}

/**
 * \brief Returns the vector of child nodes.
 *
 * \return A const reference to the vector of children.
 */
const std::vector<std::shared_ptr<NODE>> &NODE::get_children() const
{
    return m_children;
}

/**
 * \brief Sets an HTML attribute on this node.
 *
 * Stores a name-value pair in the attributes map if both are non-empty.
 * Used for attributes like class, id, href, src, etc.
 *
 * \param name The attribute name.
 * \param value The attribute value.
 */
void NODE::set_attribute(const std::string &name, const std::string &value)
{
    if (!name.empty() && !value.empty())
    {
        m_attributes[name] = value;
    }
}

/**
 * \brief Retrieves an HTML attribute value by name.
 *
 * Looks up an attribute in the attributes map. Returns empty string if
 * the attribute doesn't exist.
 *
 * \param name The attribute name to retrieve.
 * \return The attribute value, or empty string if not found.
 */
std::string NODE::get_attribute(const std::string &name) const
{
    try
    {
        return m_attributes.at(name);
    }
    catch (...)
    {
        return "";
    }
}

/**
 * \brief Sets a CSS style property on this node.
 *
 * Uses the COMPUTED_STYLE setters map to parse and apply a CSS property value.
 * Silently ignores unknown properties.
 *
 * \param name The CSS property name (e.g., "color", "font-size").
 * \param value The CSS property value (e.g., "red", "14px").
 */
void NODE::set_style(const std::string &name, const std::string &value)
{
    if (m_computed_style.setters.count(name))
    {
        m_computed_style.setters[name](m_computed_style, value);
    }
}

/**
 * \brief Returns all computed styles for this node.
 *
 * \return A COMPUTED_STYLE object containing all CSS properties and their values.
 */
COMPUTED_STYLE NODE::get_all_styles() const
{
    return m_computed_style;
}

/**
 * \brief Sets the parent node reference.
 *
 * Stores a weak pointer to the parent node to avoid circular references
 * and prevent memory leaks in the DOM tree.
 *
 * \param parent A weak pointer to the parent Node.
 */
void NODE::set_parent(std::weak_ptr<NODE> parent)
{
    m_parent = parent;
}

/**
 * \brief Retrieves the parent node.
 *
 * Locks the weak pointer to the parent and returns a shared pointer.
 * Returns nullptr if the parent has been destroyed.
 *
 * \return A shared pointer to the parent Node, or nullptr if none exists.
 */
std::shared_ptr<NODE> NODE::get_parent() const {
    return m_parent.lock();
}
