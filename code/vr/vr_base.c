#include "vr_base.h"
#include "../VrApi/Include/VrApi.h"
#include "../VrApi/Include/VrApi_Helpers.h"
#include "../VrApi/Include/VrApi_Types.h"
#include "../qcommon/q_shared.h"
#include "../qcommon/qcommon.h"
#include "../client/client.h"

//#if __ANDROID__

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wstrict-prototypes"
#pragma clang diagnostic pop

#include <EGL/egl.h>

#include <assert.h>

static engine_t vr_engine;

cvar_t *vr_worldscale = NULL;
cvar_t *vr_hudDepth = NULL;
cvar_t *vr_righthanded = NULL;
cvar_t *vr_snapturn = NULL;
cvar_t *vr_heightAdjust = NULL;
cvar_t *vr_extralatencymode = NULL;
cvar_t *vr_directionMode = NULL;
cvar_t *vr_weaponPitch = NULL;
cvar_t *vr_twoHandedWeapons = NULL;
cvar_t *vr_refreshrate = NULL;
cvar_t *vr_weaponScope = NULL;
cvar_t *vr_jumpTrigger = NULL;
cvar_t *vr_rollWhenHit = NULL;
cvar_t *vr_hudYOffset = NULL;

engine_t* VR_Init( ovrJava java )
{
	ovrInitParms initParams;
	ovrResult initResult;

	memset(&vr_engine, 0, sizeof(vr_engine));

	initParams = vrapi_DefaultInitParms(&java);
	initResult = vrapi_Initialize(&initParams);
	assert(initResult == VRAPI_INITIALIZE_SUCCESS);
	
	vr_engine.java = java;

	return &vr_engine;
}

void VR_InitCvars( void )
{
	vr_worldscale = Cvar_Get ("vr_worldscale", "32.0", CVAR_ARCHIVE);
	vr_hudDepth = Cvar_Get ("vr_hudDepth", "3", CVAR_ARCHIVE);
	vr_righthanded = Cvar_Get ("vr_righthanded", "1", CVAR_ARCHIVE);
	vr_snapturn = Cvar_Get ("vr_snapturn", "45", CVAR_ARCHIVE);
	vr_extralatencymode = Cvar_Get ("vr_extralatencymode", "1", CVAR_ARCHIVE);
	vr_directionMode = Cvar_Get ("vr_directionMode", "0", CVAR_ARCHIVE); // 0 = HMD, 1 = Off-hand
	vr_weaponPitch = Cvar_Get ("vr_weaponPitch", "-20", CVAR_ARCHIVE);
	vr_heightAdjust = Cvar_Get ("vr_heightAdjust", "0.0", CVAR_ARCHIVE);
    vr_twoHandedWeapons = Cvar_Get ("vr_twoHandedWeapons", "1", CVAR_ARCHIVE);
	vr_refreshrate = Cvar_Get ("vr_refreshrate", "0", CVAR_ARCHIVE);
    vr_weaponScope = Cvar_Get ("vr_weaponScope", "1", CVAR_ARCHIVE);
	vr_jumpTrigger = Cvar_Get ("vr_jumpTrigger", "1", CVAR_ARCHIVE);
	vr_rollWhenHit = Cvar_Get ("vr_rollWhenHit", "0", CVAR_ARCHIVE);
	vr_hudYOffset = Cvar_Get ("vr_hudYOffset", "0", CVAR_ARCHIVE);

	// Values are:  scale,right,up,forward,pitch,yaw,roll
	// VALUES PROVIDED BY SkillFur - Thank-you!
	Cvar_Get ("vr_weapon_adjustment_1", "1,-4.0,7,-10,-20,-15,0", CVAR_ARCHIVE);
	Cvar_Get ("vr_weapon_adjustment_2", "0.8,-3.0,5.5,0,0,0,0", CVAR_ARCHIVE);
	Cvar_Get ("vr_weapon_adjustment_3", "0.8,-3.3,8,3.7,0,0,0", CVAR_ARCHIVE); // shotgun
	Cvar_Get ("vr_weapon_adjustment_4", "0.75,-5.4,6.5,-4,0,0,0", CVAR_ARCHIVE);
	Cvar_Get ("vr_weapon_adjustment_5", "0.8,-5.2,6,7.5,0,0,0", CVAR_ARCHIVE);
	Cvar_Get ("vr_weapon_adjustment_6", "0.8,-3.3,6,7,0,0,0", CVAR_ARCHIVE);
	Cvar_Get ("vr_weapon_adjustment_7", "0.8,-5.5,6,0,0,0,0", CVAR_ARCHIVE);
	Cvar_Get ("vr_weapon_adjustment_8", "0.8,-4.5,6,1.5,0,0,0", CVAR_ARCHIVE);
	Cvar_Get ("vr_weapon_adjustment_9", "0.8,-5.5,6,0,0,0,0", CVAR_ARCHIVE);

	//Team Arena Weapons
	Cvar_Get ("vr_weapon_adjustment_10", "0.8,-5.5,6,0,0,0,0", CVAR_ARCHIVE);
	Cvar_Get ("vr_weapon_adjustment_11", "0.8,-5.5,6,0,0,0,0", CVAR_ARCHIVE);
	Cvar_Get ("vr_weapon_adjustment_12", "0.8,-5.5,6,0,0,0,0", CVAR_ARCHIVE);

}

void VR_Destroy( engine_t* engine )
{
	if (engine == &vr_engine) {
		vrapi_Shutdown();
	}
}

void VR_EnterVR( engine_t* engine, ovrJava java ) {
	if (!engine->ovr) {
		ovrModeParms modeParams = vrapi_DefaultModeParms(&java);
		modeParams.Display = (size_t)eglGetCurrentDisplay();
		modeParams.WindowSurface = (size_t)eglGetCurrentSurface(EGL_DRAW);
		modeParams.ShareContext = (size_t)eglGetCurrentContext();

		engine->ovr = vrapi_EnterVrMode(&modeParams);
		engine->frameIndex = 0;

		vrapi_SetTrackingSpace(engine->ovr, VRAPI_TRACKING_SPACE_LOCAL_FLOOR);

        vrapi_SetClockLevels(engine->ovr, 4, 4);
	}
}

void VR_LeaveVR( engine_t* engine ) {
	if (engine->ovr) {
		vrapi_LeaveVrMode(engine->ovr);
		engine->ovr = NULL;
	}
}

engine_t* VR_GetEngine( void ) {
	return &vr_engine;
}

bool VR_useScreenLayer( void )
{
	//intermission is never full screen
    if ( cl.snap.ps.pm_type == PM_INTERMISSION )
    {
        return qfalse;
    }

    int keyCatcher = Key_GetCatcher( );
	return (bool)( clc.state == CA_CINEMATIC ||
			( keyCatcher & (KEYCATCH_UI | KEYCATCH_CONSOLE) ));
}
//#endif
