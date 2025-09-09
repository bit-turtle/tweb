#include <openssl/x509v3.h>
#define TB_IMPL
#include "lib/termbox2.h"
#undef TB_IMPL
#include "instance.h"

#include <vector>

std::string search_engine = "https://lite.duckduckgo.com/lite?q=";
std::string home_page = "https://www.rfc-editor.org/rfc/rfc2549.txt";

struct Tab {
    Instance* instance;
    int scroll = 0;
};

int main() {
    
    tb_init();

    std::vector<Tab> tabs;
    unsigned active_tab = 0;

    tabs.push_back({ new Instance(home_page) });

    struct tb_event e;
    
    while (tabs.size() > 0) {

        Tab* tab = &tabs.at(active_tab);

        // Render
        tb_clear();

        tab->instance->con->render(&tab->scroll);

        tb_print(tb_width()-std::to_string(active_tab).length(), tb_height()-1, TB_MAGENTA, 0, std::to_string(active_tab).c_str());

        tb_present();

        tb_poll_event(&e);

        if (e.type == TB_EVENT_KEY) {

            // Exit
            if (e.key == TB_KEY_CTRL_C || e.ch == 'q')
                break;

            // Back/Forward
            else if (e.key == TB_KEY_ARROW_LEFT || e.ch == 'j') {
                tb_clear();
                tb_print(0, 0, TB_RED, 0, "Loading... [Back]");
                tb_present();
                tab->scroll = 0;
                tab->instance->back();
            }
            else if (e.key == TB_KEY_ARROW_RIGHT || e.ch == 'k') {
                tb_clear();
                tb_print(0, 0, TB_RED, 0, "Loading... [Forward]");
                tb_present();
                tab->scroll = 0;
                tab->instance->forward();
            }

            // Scroll
            else if (e.key == TB_KEY_ARROW_UP)
                tab->scroll--;
            else if (e.key == TB_KEY_ARROW_DOWN)
                tab->scroll++;
            else if (e.key == TB_KEY_PGUP)
                tab->scroll-=tb_height();
            else if (e.key == TB_KEY_PGDN)
                tab->scroll+=tb_height();

            // Create New Tab
            else if (e.ch == 't') {
                tb_clear();
                tb_print(0, 0, TB_RED, 0, "Loading... [NewTab]");
                tb_present();
                tabs.insert(tabs.begin()+active_tab, { new Instance(home_page) });
                active_tab++;
            }
            // Close Tab
            else if (e.ch == 'w') {
                delete tabs.at(active_tab).instance;
                tabs.erase(tabs.begin()+active_tab);
                if (active_tab>tabs.size()-1)
                    active_tab = tabs.size()-1;
            }

            // Change Active Tab
            else if (e.key == TB_KEY_TAB || e.ch == 'l')
                active_tab = (active_tab<tabs.size()-1) ? active_tab+1 : active_tab;
            else if (e.key == TB_KEY_BACK_TAB || e.ch == 'h')
                active_tab = (active_tab>0) ? active_tab-1 : active_tab;

            // Go to website
            else if (e.ch == 'g' || e.ch == 's') {
                int offset = 5;
                bool search = false;
                std::string prompt = "Url: ";
                if (e.ch == 's') {
                    offset = 8;
                    search = true;
                    prompt = "Search: ";
                }
                tb_print(0, tb_height()-1, TB_CYAN, 0, prompt.c_str());
                tb_set_cursor(offset, tb_height()-1);
                tb_present();
                std::string cmd;
                int x = 0;
                int state = 0;
                while (state == 0) {
                    tb_poll_event(&e);
                    if (e.type == TB_EVENT_KEY) {
                        if (e.key == TB_KEY_CTRL_C)
                            state = -1;
                        else if (e.key == TB_KEY_ENTER) {
                            tb_clear();
                            tb_hide_cursor();
                            tb_print(0, 0, TB_RED, 0, "Loading...");
                            state = 1;
                        }
                        else if (e.key == TB_KEY_ARROW_DOWN)
                            x = 0;
                        else if (e.key == TB_KEY_ARROW_UP)
                            x = cmd.length()-1;
                        else if (e.key == TB_KEY_ARROW_LEFT)
                            x = (x>0) ? x-1 : x;
                        else if (e.key == TB_KEY_ARROW_RIGHT)
                            x = (x<cmd.length()-1) ? x+1 : x;
                        else if (e.key == TB_KEY_BACKSPACE || e.key == TB_KEY_BACKSPACE2) {
                            if (x > 0) {
                                x--;
                                cmd.erase(x);
                                tb_set_cell(offset+x, tb_height()-1, ' ', 0, 0);
                            }
                            tb_set_cursor(offset+x, tb_height()-1);
                        }
                        else {
                            cmd.insert(cmd.begin()+x, e.ch);
                            tb_set_cell(offset+x, tb_height()-1, e.ch, 0, 0);
                            x++;
                            tb_set_cursor(offset+x, tb_height()-1);
                        }
                    }
                    tb_present();
                }
                if (state == -1)
                    break;
                tab->scroll = 0;
                if (search) {
                    std::ostringstream query;
                    query << search_engine << cmd;
                    tabs.at(active_tab).instance->open(query.str());
                }
                else
                    tabs.at(active_tab).instance->open(cmd);
            }
            
        }

        // Limit Scroll
        if (tab->scroll < 0)
            tab->scroll = 0;

    }

    tb_shutdown();

    return 0;
}
