#define TB_IMPL
#include "lib/termbox2.h"
#undef TB_IMPL
#include "instance.h"

#include <vector>

struct Tab {
    Instance* instance;
    int scroll = 0;
};

int main() {
    
    tb_init();

    std::vector<Tab> tabs;
    unsigned active_tab = 0;

    tabs.push_back({ new Instance("bit-turtle.github.io") });

    struct tb_event e;
    
    while (tabs.size() > 0) {

        Tab* tab = &tabs.at(active_tab);

        // Render
        tb_clear();

        tab->instance->con->render(&tab->scroll);

        tb_present();

        tb_poll_event(&e);

        if (e.type == TB_EVENT_KEY) {

            // Exit
            if (e.key == TB_KEY_CTRL_C || e.ch == 'q')
                break;

            // Back/Forward
            else if (e.key == TB_KEY_ARROW_LEFT)
                tab->instance->back();
            else if (e.key == TB_KEY_ARROW_RIGHT)
                tab->instance->forward();

            // Scroll
            else if (e.key == TB_KEY_ARROW_UP)
                tab->scroll--;
            else if (e.key == TB_KEY_ARROW_DOWN)
                tab->scroll++;
            else if (e.key == TB_KEY_PGUP)
                tab->scroll-=20;
            else if (e.key == TB_KEY_PGDN)
                tab->scroll+=20;
            
        }

        // Limit Scroll
        if (tab->scroll < 0)
            tab->scroll = 0;

    }

    tb_shutdown();

    return 0;
}
