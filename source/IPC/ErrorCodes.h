#pragma once
#include <switch.h>

//TODO check for conflicts, remove unused ones
#define LAYOFF_MODULE 66

#define ERR_CLIENT_ALREADY_REGISTERED	MAKERESULT(LAYOFF_MODULE, 1)
#define ERR_CLIENT_NOT_REGISTERED		MAKERESULT(LAYOFF_MODULE, 4)
#define ERR_UNKNOWN_UI_TYPE				MAKERESULT(LAYOFF_MODULE, 3)
#define ERR_INVALID_ID					MAKERESULT(LAYOFF_MODULE, 4)
#define ERR_BUFFER_TOO_SMALL			MAKERESULT(LAYOFF_MODULE, 5)

#define ERR_INVALID_NAME				MAKERESULT(LAYOFF_MODULE, 5)