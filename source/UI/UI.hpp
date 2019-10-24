#include <switch.h>
#include <lvgl.h>

Framebuffer fb;
NWindow *win;

Thread tick;
static lv_disp_buf_t disp_buf;
static lv_color_t buf[LV_HOR_RES_MAX * 10];

lv_disp_drv_t disp_drv;
lv_indev_drv_t indev_drv_touch;

void tickThread() {
    while(true) {
        lv_tick_inc(10);
        svcSleepThread(1e+7);
    }
}

void my_disp_flush(lv_disp_drv_t * disp, const lv_area_t * area, lv_color_t * color_p)
{
    u32 stride;
    u32* framebuf = (u32*) framebufferBegin(&fb, &stride);

    int32_t x, y;
    for(y = area->y1; y <= area->y2; y++) {
        for(x = area->x1; x <= area->x2; x++) {
            u32 pos = y * stride / sizeof(u32) + x;
            u32 pixel =  color_p->ch.alpha << 24 | color_p->ch.blue << 16 | color_p->ch.green << 8 | color_p->ch.red;
            framebuf[pos] = pixel;
            color_p++;
        }
    }
    framebufferEnd(&fb);
    lv_disp_flush_ready(disp);
}

bool touchscreen_read(lv_indev_drv_t * indev_driver, lv_indev_data_t * data)
{
    /*Save the state and save the pressed coordinate*/
    touchPosition touchPos;
    data->state = (hidTouchCount() > 0) ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL; 
    if(data->state == LV_INDEV_STATE_PR) hidTouchRead(&touchPos, 0);
   
    /*Set the coordinates (if released use the last pressed coordinates)*/
    data->point.x = touchPos.px;
    data->point.y = touchPos.py;

    return false; /*Return `false` because we are not buffering and no more data to read*/
}

void UIInit(NWindow *window) {
    /* ==Switch FB init== */
    win = window;
    if(!win)
	{
		fatalSimple(MAKERESULT(255,120));
	}

    Result rc = framebufferCreate(&fb, win, LV_HOR_RES_MAX, LV_VER_RES_MAX, PIXEL_FORMAT_RGBA_8888, 2);
	if (R_FAILED(rc))
		fatalSimple(rc);
    
    rc = framebufferMakeLinear(&fb);
	if (R_FAILED(rc))
		fatalSimple(rc);
    
    /* ==LVGL library init== */
    lv_init();
    threadCreate(&tick, (ThreadFunc)tickThread, NULL, NULL, 0x2000, 0x3E, -2);
    threadStart(&tick);

    lv_style_scr.body.main_color     = LV_COLOR_LIME;
    lv_style_scr.body.grad_color     = LV_COLOR_LIME;
    
    /* ==LVGL disp driver init== */
    lv_disp_buf_init(&disp_buf, buf, NULL, LV_HOR_RES_MAX * 10);
    lv_disp_drv_init(&disp_drv);
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.buffer = &disp_buf;
    lv_disp_drv_register(&disp_drv);

    /* ==LVGL input init */
    // Touch
    lv_indev_drv_init(&indev_drv_touch);
    indev_drv_touch.type = LV_INDEV_TYPE_POINTER;
    indev_drv_touch.read_cb = touchscreen_read;
    lv_indev_drv_register(&indev_drv_touch);
}

void UIUpdate() {
    lv_task_handler();
}