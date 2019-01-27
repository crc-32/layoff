#include <string>
using namespace std;
class Notification {
    public:
        string title;
        string subtitle;
        bool isSimple = false;
        
        Notification(string title, string subtitle) {
            this->title = title;
            this->subtitle = subtitle;
        }
        Notification(string text) {
            this->title = text;
            isSimple = true;
        }
}