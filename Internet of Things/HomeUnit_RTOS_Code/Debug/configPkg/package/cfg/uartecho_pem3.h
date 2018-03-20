/*
 *  Do not modify this file; it is automatically 
 *  generated and any modifications will be overwritten.
 *
 * @(#) xdc-B06
 */

#include <xdc/std.h>

#include <ti/sysbios/knl/Task.h>
extern const ti_sysbios_knl_Task_Handle inithandle;

#include <ti/sysbios/knl/Task.h>
extern const ti_sysbios_knl_Task_Handle ledblinkhandle;

#include <ti/sysbios/knl/Task.h>
extern const ti_sysbios_knl_Task_Handle temphandle;

#include <ti/sysbios/knl/Swi.h>
extern const ti_sysbios_knl_Swi_Handle BULB_1;

#include <ti/sysbios/knl/Swi.h>
extern const ti_sysbios_knl_Swi_Handle BULB_2;

#include <ti/sysbios/knl/Task.h>
extern const ti_sysbios_knl_Task_Handle bulbdatahandle;

#include <ti/sysbios/knl/Semaphore.h>
extern const ti_sysbios_knl_Semaphore_Handle bulbsem;

extern int xdc_runtime_Startup__EXECFXN__C;

extern int xdc_runtime_Startup__RESETFXN__C;

