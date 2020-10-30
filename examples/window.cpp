#include <AceEngine/AceEngine.hpp>

ACE::window window;
SDL_Event event;

int main()
{
    window.create("window", ACE_WINDOW_POS_CENTERED, ACE::vector2<int>(800, 800));

    while (window.is_open() ||
           /* you can also check with this */ window != NULL)
    {
        while (SDL_PollEvent(&event))
        {
            if (window.quit(event))
                window.close();
        }

        window.clear();
        window.display();
    }
}