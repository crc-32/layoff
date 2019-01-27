#include <switch.h>
using namespace std;
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