#include "space_chief/aowindow.cxx"
#include "space_chief/chief.cxx"

#include "spaces/enter_name_space.cxx"


int main() {
    AOWindow::global({1280u, 720u}, "new game");
    Chief::global()
        .setSpace<EnterNameSpace>()
        .run();
}