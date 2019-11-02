#include <switch.h>

static ViDisplay g_viDisplay;
static ViLayer g_viLayer;
static NWindow g_defaultWin;

NWindow* nwindowGet(void)
{
    return &g_defaultWin;
}

extern "C" u64 __nx_vi_layer_id;
void win_init(void)
{
    Result rc;
    rc = viInitialize(ViServiceType_Manager);
    if (R_FAILED(rc)) fatalSimple(MAKERESULT(255, 1));
    if (R_SUCCEEDED(rc)) {
        rc = viOpenDefaultDisplay(&g_viDisplay);
        if (R_FAILED(rc)) fatalSimple(MAKERESULT(255, 2));
        if (R_SUCCEEDED(rc)) {
            u64 aruid;
            appletGetAppletResourceUserId(&aruid);
            rc = viCreateManagedLayer(&g_viDisplay, (ViLayerFlags)0, aruid, &__nx_vi_layer_id);
            if (R_FAILED(rc)) fatalSimple(rc);
            if (R_SUCCEEDED(rc)) {
                rc = viCreateLayer(&g_viDisplay, &g_viLayer);
                if (R_SUCCEEDED(rc)) {
                    rc = viSetLayerScalingMode(&g_viLayer, ViScalingMode_FitToLayer);
                    if (R_FAILED(rc)) fatalSimple(rc);
                    rc = viSetLayerZ(&g_viLayer, 100);
                    if (R_FAILED(rc)) fatalSimple(rc);
                    rc = viSetLayerSize(&g_viLayer, HOR_LAYER_SIZE, VERT_LAYER_SIZE);
                    if (R_FAILED(rc)) fatalSimple(rc);
                    rc = viSetLayerPosition(&g_viLayer, 1920 - HOR_LAYER_SIZE, 0);
                    if (R_FAILED(rc)) fatalSimple(rc);
                    if (R_SUCCEEDED(rc)) {
                        rc = nwindowCreateFromLayer(&g_defaultWin, &g_viLayer);
                    }
                }
                if (R_FAILED(rc))
                    viCloseLayer(&g_viLayer);
            }
            if (R_FAILED(rc))
                viCloseDisplay(&g_viDisplay);
        }
        if (R_FAILED(rc))
            viExit();
    }
    if (R_FAILED(rc))
        //fatalSimple(MAKERESULT(Module_Libnx, LibnxError_BadGfxInit));
        fatalSimple(rc);
}

void win_exit(void)
{
    nwindowClose(&g_defaultWin);
    viCloseLayer(&g_viLayer);
    viCloseDisplay(&g_viDisplay);
    viExit();
}