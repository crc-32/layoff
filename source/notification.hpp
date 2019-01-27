#include <string>
#include "rect.hpp"
using namespace std;
class Notification {
    public:
        string title;
        string subtitle;
        Rect *rect;
        bool isSimple = false;
        u32 bgCol = RGBA8(10, 10, 10, 128);
        
        Notification(string title, string subtitle) {
            this->title = title;
            this->subtitle = subtitle;
            this->rect = new Rect(0, 0, 250, 80, bgCol);
        }
        Notification(string text) {
            this->title = text;
            this->rect = new Rect(0, 0, 250, 80, bgCol);
            isSimple = true;
        }
};