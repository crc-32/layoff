#include <switch.h>
class Drawing {
    private:
        ViDisplay display;
        ViLayer layer;
        NWindow win;
        Framebuffer fb;

    public:
        Drawing() {};
        void Setup();
        void Exit();
        void Test();
};