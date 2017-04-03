#ifndef SVG_CONVERTER_PARSING_CONTEXT_G_H_
#define SVG_CONVERTER_PARSING_CONTEXT_G_H_

#include "graphics_element.h"

/**
 * Context for parsing <g> elements.
 */
template <class Exporter>
class GContext : public GraphicsElementContext<Exporter> {
 public:
    template <class ParentContext>
    explicit GContext(ParentContext& parent)
        : GraphicsElementContext<Exporter>{parent} {}

    /**
     * Used by the `GraphicsElementContext(const ParentContext&)` constructor.
     * @return Viewport for child elements.
     */
    const Viewport& inner_viewport() const { return this->viewport_; }
};

#endif  // SVG_CONVERTER_PARSING_CONTEXT_G_H_