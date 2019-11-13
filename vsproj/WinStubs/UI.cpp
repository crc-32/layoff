#include "../source/UI/UI.hpp"
#include <MiniFB.h>
#include <stdexcept>
#include <CrossSwitch.h>

void svcSleepThread(u64 nano)
{
	_sleep(nano * 1e-7);
}

using namespace std;

struct Window* window;
u32 pixels[DEFAULT_WIN_WIDTH * DEFAULT_WIN_HEIGHT];

touchPosition t;
static u64 kHeld, kHeldOld;
static void keyboard(Window* window, Key key, KeyMod mod, bool isPressed) {
	u64 target = 0;

	if (key == KB_KEY_ESCAPE)
		target = KEY_A;
	else if (key == KB_KEY_UP)
		target = KEY_DUP;
	else if (key == KB_KEY_DOWN)
		target = KEY_DDOWN;
	else if (key == KB_KEY_LEFT)
		target = KEY_DLEFT;
	else if (key == KB_KEY_RIGHT)
		target = KEY_DRIGHT;
	else if (key == KB_KEY_A)
		target = KEY_A;
	else if (key == KB_KEY_B)
		target = KEY_B;

	if (target)
		kHeld = isPressed ? (kHeld | target) : (kHeld & ~target);
}

bool Clicked = true;
static void mouse_btn(Window* window, MouseButton button, KeyMod mod, bool isPressed) {
	if (button == MOUSE_BTN_1)
		Clicked = isPressed;
}

// Use wisely this event. It can be sent too often
static void mouse_move(Window* window, int x, int y) {
	t.px = x;
	t.py = y;
}

u64 hidKeysHeld(int)
{
	return kHeld;
}

u64 hidKeysDown(int)
{
	return kHeld & ~kHeldOld;
}

int hidTouchCount()
{
	return Clicked ? 1 : 0;
}

void hidTouchRead(touchPosition* a, int _)
{
	a->px = t.px;
	a->py = t.py;
}

void UIInit()
{
	window = mfb_open_ex("Layoff", DEFAULT_WIN_WIDTH, DEFAULT_WIN_HEIGHT, 0);
	if (!window)
		throw runtime_error("couldn't init win");

	mfb_keyboard_callback(window, keyboard);
	mfb_mouse_button_callback(window, mouse_btn);
	mfb_mouse_move_callback(window, mouse_move);

	ImguiInitialize();

	ImGuiIO& io = ImGui::GetIO();
	//ImFontAtlasFlags_NoPowerOfTwoHeight may be needed when/if we switch to hardware rendering
	io.Fonts->Flags |= ImFontAtlasFlags_NoPowerOfTwoHeight | ImFontAtlasFlags_NoMouseCursors;
	io.Fonts->AddFontFromFileTTF("opensans.ttf", 25.0f);
	ImGuiFreeType::BuildFontAtlas(io.Fonts, 0u);

	imgui_sw::bind_imgui_painting();
}

void FrameStart()
{
	ImGui::NewFrame();
}

void ClearFramebuffer()
{
	memset(pixels, 0, sizeof(u32) * DEFAULT_WIN_WIDTH * DEFAULT_WIN_HEIGHT);
	mfb_update(window, pixels);
}

static inline u32 swap_uint32(u32 val)
{
	return ((val & 0x000000FF) << 16) | (val & 0x0000FF00) | ((val & 0x00FF0000) >> 16);
}

void FrameEnd()
{
	kHeldOld = kHeld;

	ImGui::Render();

	memset(pixels, 0, sizeof(u32) * DEFAULT_WIN_WIDTH * DEFAULT_WIN_HEIGHT);
	imgui_sw::paint_imgui(pixels, DEFAULT_WIN_WIDTH, DEFAULT_WIN_HEIGHT);

	for (int i = 0; i < DEFAULT_WIN_WIDTH * DEFAULT_WIN_HEIGHT; i++)
		pixels[i] = swap_uint32(pixels[i]);

	mfb_update(window, pixels);
}

void UIExit() 
{
	mfb_close(window);
}