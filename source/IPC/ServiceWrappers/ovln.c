#define NX_SERVICE_ASSUME_NON_DOMAIN
#include <string.h>
#include "ovln.h"

static Service g_ovlnSrv;
static Service g_irecSrv;

static Result _getIRec() {
	return serviceDispatch(&g_ovlnSrv, 0,
        .out_num_objects = 1,
        .out_objects = &g_irecSrv,
    );
}

static Result _regIRec() {
	struct {
		char name[0x10];
	} name = { 0 };
	strcpy(name.name, "overlay");
	//serviceDispatchIn is a macro, passing a string will break cause it tries to get the address, also sizeof but it's not the case here
	//using a struct we get a "value type" string
	return serviceDispatchIn(&g_irecSrv, 0, name);
}

Result ovlnInitialize() {
    Result rc = smGetService(&g_ovlnSrv, "ovln:rcv");

    if (R_SUCCEEDED(rc)) {
        rc = _getIRec();
		if (R_SUCCEEDED(rc))
			rc = _regIRec();
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

Result ovlnIReceiverGetReceiveEventHandle(Event *out) {
	Handle tmp_handle;

	Result rc = serviceDispatch(&g_irecSrv, 2,
        .out_handle_attrs = { SfOutHandleAttr_HipcCopy },
        .out_handles = &tmp_handle,
    );
    if (R_SUCCEEDED(rc)) eventLoadRemote(out, tmp_handle, true);
    return rc;
}

Result ovlnIReceiverReceiveWithTick(OvlnNotificationWithTick*out)
{
	return serviceDispatchOut(&g_irecSrv, 4, *out);
}