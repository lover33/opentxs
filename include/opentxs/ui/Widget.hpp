// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef OPENTXS_UI_WIDGET_HPP
#define OPENTXS_UI_WIDGET_HPP

#include "opentxs/Forward.hpp"

#include <string>

#ifdef SWIG
// clang-format off
%extend opentxs::ui::Widget {
    std::string WidgetID() const
    {
        return $self->WidgetID()->str();
    }
}
%ignore opentxs::ui::Widget::WidgetID;
%rename(UIWidget) opentxs::ui::Widget;
// clang-format on
#endif  // SWIG

namespace opentxs
{
namespace ui
{
class Widget
{
public:
    EXPORT virtual OTIdentifier WidgetID() const = 0;

    EXPORT virtual ~Widget() = default;

protected:
    Widget() = default;

private:
    Widget(const Widget&) = delete;
    Widget(Widget&&) = delete;
    Widget& operator=(const Widget&) = delete;
    Widget& operator=(Widget&&) = delete;
};
}  // namespace ui
}  // namespace opentxs
#endif
