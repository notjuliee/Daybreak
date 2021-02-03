#include <nana/gui.hpp>
#include <nana/gui/widgets/textbox.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/widgets/group.hpp>

#ifdef _MSC_VER
int WinMain() {
#else
int main() {
#endif
    nana::form fm;
    fm.caption("Daybreak Launcher");

    nana::group options {fm};
    options.caption("Options");
    options.div("vert<engine>");

    auto *lb = options.create_child<nana::textbox>("engine");
    lb->multi_lines(false);

    nana::place layout {fm};
    layout.div("fit y vert<options><tools>");
    layout["options"] << options;
    layout.collocate();

    fm.show();

    nana::exec();
    return 0;
}