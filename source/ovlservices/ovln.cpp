#define NX_SERVICE_ASSUME_NON_DOMAIN
#include <switch.h>
#include "ovln.h"

static Service g_ovlnSrv;
static Service g_irecSrv;

Result _getIRec() {
	return serviceDispatch(&g_ovlnSrv, 0,
        .out_num_objects = 1,
        .out_objects = &g_irecSrv,
    );
}

Result _regIRec() {
	return serviceDispatchIn(&g_irecSrv, 0, "overlay");
}

Result ovlnInitialize() {
    Result rc = smGetService(&g_ovlnSrv, "ovln:rcv");

    if (R_SUCCEEDED(rc)) {
        rc = _getIRec();
        if (R_SUCCEEDED(rc)) {
            rc = _regIRec();
        }
    }

    if (R_FAILED(rc)) {
        ovlnExit();
    }

    return rc;
}

void ovlnExit() {
    serviceClose(&g_ovlnSrv);
    serviceClose(&g_irecSrv);
}

Result ovlnIReceiverGetEvent(Event *out) {
	Handle tmp_handle;

	Result rc = serviceDispatch(&g_irecSrv, 2,
        .in_send_pid = true,
        .out_handle_attrs = { SfOutHandleAttr_HipcCopy },
        .out_handles = &tmp_handle,
    );
    if (R_SUCCEEDED(rc)) eventLoadRemote(out, tmp_handle, true);
    return rc;
}

Result ovlnIReceiverGetNotification(IReceiverNotification *out)
{
	struct {
		u64 magic;
		u64 result;
		u16 type;
		u8 off[6];
		u16 content;
	} *resp = {0};

	Result rc = serviceDispatchOut(&g_irecSrv, 4, resp);
	if(R_SUCCEEDED(rc)) {
		out->content = resp->content;
		out->type = (resp->type>>8) | (resp->type<<8);
	}
    return rc;
}