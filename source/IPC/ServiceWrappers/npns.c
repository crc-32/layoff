#include "npns.h"

Service g_npnsSrv;
Service g_iNRecSrv;

Result _getINotifRec() {
    return serviceDispatch(&g_npnsSrv, 120,
    .out_num_objects = 1,
    .out_objects = &g_iNRecSrv
    );
}

Result npnsInitialize() {
    Result rc = smGetService(&g_npnsSrv, "npns:u");

    if (R_SUCCEEDED(rc)) {
        rc = _getINotifRec();
    }

    if (R_FAILED(rc)) {
        npnsExit();
    }

    return rc;
}

void npnsExit() {
    serviceClose(&g_npnsSrv);
    serviceClose(&g_iNRecSrv);
}