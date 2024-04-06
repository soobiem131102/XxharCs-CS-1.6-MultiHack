(trojanvirustotallynoti=vi===virus
		return;*/
	/*if(cvar.aimingmethod == 1)
	{*/
		if( !vPlayers[pEnt->index].bGotHead )
		{
			int iIndex = pEnt->index;

			model_s		    *pModel			= pStudio->SetupPlayerModel( iIndex );
			studiohdr_t     *pStudioHeader	= ( studiohdr_t* )pStudio->Mod_Extradata( pModel );
			mstudiobbox_t   *pStudioBox;
			TransformMatrix *pBoneTransform = ( TransformMatrix* )pStudio->StudioGetBoneTransform( );	

			vec3_t vMin, vMax;
			pStudioBox = ( mstudiobbox_t* )( ( byte* )pStudioHeader + pStudioHeader->hitboxindex );

			//Head 11 bone 7 | Low Head 9 bone 5 | Chest 8 bone 4 | Stomach 7 bone 3
			int i = 11;
			
			VectorTransform(pStudioBox[i].bbmin, (*pBoneTransform)[pStudioBox[i].bone], vMin);
			VectorTransform(pStudioBox[i].bbmax, (*pBoneTransform)[pStudioBox[i].bone], vMax);
			vPlayers[iIndex].vHitbox	= ( vMin + vMax ) * 0.5f;
			vPlayers[iIndex].bGotHead	= true;		
		}
	/*}*/
	/*if(cvar.aimingmethod == 2)
	{
		int ax = pEnt->index;
		if(!vPlayers[ax].bGotHead)
		{
			vec3_t pos;
	//		studiohdr_t* pStudioHeader = (studiohdr_t*)oEngStudio.Mod_Extradata( pEnt->model );
			TransformMatrix*  pbonetransform = (TransformMatrix*)pStudio->StudioGetBoneTransform();
			//Head 11 bone 7 | Low Head 9 bone 5 | Chest 8 bone 4 | Stomach 7 bone 3
			int i = 7;
			if (cvar.aimspot == 1) i = 7;
			else if (cvar.aimspot == 2) i = 5;
			else if (cvar.aimspot == 3) i = 4;
			pos[ 0 ] = (*pbonetransform)[ i ][ 0 ][ 3 ];
			pos[ 1 ] = (*pbonetransform)[ i ][ 1 ][ 3 ];
			pos[ 2 ] = (*pbonetransform)[ i ][ 2 ][ 3 ];
			VectorCopy(pos, vPlayers[ax].vHitbox);
			vPlayers[ax].bGotHead = true;
		}
	}*/
}

//==============================================================================

void CAimbot::CalculateAimingView( void )
{
	float view[3];
	vec3_t vecEnd, up, right, forward, EntViewOrg, playerAngles;

	if (!vPlayers[iTarget].fixHbAim) {VectorCopy(vPlayers[iTarget].vHitbox,EntViewOrg);}	
	else							 {VectorCopy(vPlayers[iTarget].origin(),EntViewOrg);}	

	// calculate angle vectors
	playerAngles[0]=0;
	playerAngles[1]=vPlayers[iTarget].getEnt()->angles[1];
	playerAngles[2]=0;
	gEngfuncs.pfnAngleVectors (playerAngles, forward, right, up);
	forward[2] = -forward[2];
		
	EntViewOrg = EntViewOrg + forward * 0.5;
	EntViewOrg = EntViewOrg + up * 2.5;
	EntViewOrg = EntViewOrg + right * 0.0;

	view[0] = EntViewOrg[0] - me.pmEyePos[0];
	view[1] = EntViewOrg[1] - me.pmEyePos[1];
	view[2] = EntViewOrg[2] - me.pmEyePos[2];

	VectorAngles(view,aim_viewangles);
	aim_viewangles[0] *= -1;

	if (aim_viewangles[0]>180) aim_viewangles[0]-=360;
	if (aim_viewangles[1]>180) aim_viewangles[1]-=360;
}
//==============================================================================
int CorrectGunX() 
{ 
   int currentWeaponID = GetCurWeaponID();
   if (currentWeaponID == WEAPONLIST_SG550 || currentWeaponID == WEAPONLIST_G3SG1 || currentWeaponID == WEAPONLIST_SCOUT || currentWeaponID == WEAPONLIST_AWP)
   { return 3; }

   if (currentWeaponID == WEAPONLIST_AUG || currentWeaponID == WEAPONLIST_UNKNOWN1 || currentWeaponID == WEAPONLIST_UNKNOWN2 ||  currentWeaponID == WEAPONLIST_DEAGLE || currentWeaponID == WEAPONLIST_SG552 || currentWeaponID == WEAPONLIST_AK47) 
   { return 2; }
    
   { return 1; }
}
//====================================================================================
bool CAimbot::pathFree(float* xfrom,float* xto)
{ 
   int pathtest; 
   pmtrace_t tr; 
   gEngfuncs.pEventAPI->EV_SetTraceHull( 2 ); 
   gEngfuncs.pEventAPI->EV_PlayerTrace( xfrom, xto, PM_GLASS_IGNORE, me.ent->index, &tr ); 
   pathtest = (tr.fraction == 1.0); 
   if (!pathtest && CorrectGunX()) 
   { 
      pathtest = CanPenetrate(xfrom, xto, CorrectGunX()); 
   } 
   return pathtest; 
} 
//====================================================================================

bool CAimbot::TargetRegion(int ax)
{
	vec3_t vecEnd, up, right, forward, EntViewOrg,playerAngles;
	cl_entity_s* ent = vPlayers[ax].getEnt();
	
    // calculate angle vectors
	playerAngles[0]=0;
	playerAngles[1]=ent->angles[1];
	playerAngles[2]=0;
    gEngfuncs.pfnAngleVectors (playerAngles, forward, right, up);
	forward[2] = -forward[2];

	if (vPlayers[ax].bGotHead)	{VectorCopy(vPlayers[ax].vHitbox,EntViewOrg);vPlayers[ax].fixHbAim=false;}	
	else						{VectorCopy(vPlayers[ax].origin(),EntViewOrg);vPlayers[ax].fixHbAim=true;}		

	EntViewOrg = EntViewOrg + forward * 0.5;
	EntViewOrg = EntViewOrg + up * 2.5;
    EntViewOrg = EntViewOrg + right * 0.0;
		
	if(pathFree(me.pmEyePos,EntViewOrg) || pathFree(me.pmEyePos, ent->origin)) return true;

	return false;
}

//==============================================================================
/*
bool isValidEnt(cl_entity_s *ent) 
{
	if(ent && (ent != gEngfuncs.GetLocalPlayer())  && ent->player && !ent->curstate.spectator && ent->curstate.solid && !(ent->curstate.messagenum < gEngfuncs.GetLocalPlayer()->curstate.messagenum)) 
		return true;
	else 
		return false;
}*/
//==============================================================================
void CAimbot::FindTarget( void )
{
	if (!me.alive) return;	

	SetTarget(-1);

	for (int ax=0;ax<MAX_VPLAYERS;ax++)
	{
		if  ( vPlayers[ax].isUpdatedAddEnt() && vPlayers[ax].canAim && vPlayers[ax].isAlive()) // No Dead People
		{
			if( !HasTarget() )											{ SetTarget(ax); continue; }
			if( vPlayers[ax].fovangle <   vPlayers[iTarget].fovangle )	{ SetTarget(ax);	  	   }		
		} 
	}
}

//==============================================================================

float CAimbot::calcFovAngle(const float* origin_viewer, const float* angle_viewer, const float* origin_target)
{
	double vec[3], view[3];
	double dot;	
	
	view[0] = origin_target[0] - origin_viewer[0];
	view[1] = origin_target[1] - origin_viewer[1];
	view[2] = origin_target[2] - origin_viewer[2];

	dot = sqrt(view[0] * view[0] + view[1] * view[1] + view[2] * view[2]);
	dot = 1/dot;

	vec[0] = view[0] * dot;
	vec[1] = view[1] * dot;
	vec[2] = view[2] * dot;
	
	view[0] =  sin((angle_viewer[1] + 90) * (M_PI / 180));
	view[1] = -cos((angle_viewer[1] + 90) * (M_PI / 180));
	view[2] = -sin( angle_viewer[0] * (M_PI / 180));	
	
	dot = view[0] * vec[0] + view[1] * vec[1] + view[2] * vec[2];
	
	// dot to angle:
	return (float)((1.0-dot)*180.0); 
}

//==============================================================================

bool CAimbot::CheckTeam(int ax)
{
	if (me.team != vPlayers[ax].team) return true;
	return false;
}

bool bAim = false;
char* gGetWeaponName( int weaponmodel );
int DoHLHAiming(int eventcode)
{
	char *szWeapon;
	UpdateMe();
	szWeapon = gGetWeaponName(me.ent->curstate.weaponmodel);
	if(strstr(szWeapon, "nade") || strstr(szWeapon, "c4") || strstr(szWeapon, "flashbang")) return 1;
	if (eventcode == 1)
	{
		bAim = true;
		gEngfuncs.pfnClientCmd("+attack");	
		return 0;
	}
	else
	{
		bAim = false;
		gEngfuncs.pfnClientCmd("-attack");
		return 1;
	}
}
//==============================================================================
/*extern float gSpeed;
int DoSpeed(int eventcode)
{
	if (eventcode == 1)
	{
		if (cvar.knivespeed && IsCurWeaponKnife())	gSpeed = 20.0f;
		else										gSpeed = cvar.speed;		
	}
	else
		gSpeed = 0.0;
	return 0;
}*/

//==============================================================================
int getSeqInfo(int ax);
bool CAimbot::IsShielded(int ax)
{
	int seqinfo = getSeqInfo(ax);
	if (seqinfo & SEQUENCE_RELOAD)
		return false;
	if (seqinfo & SEQUENCE_SHIELD)
		return true;
	return false;
}

//==============================================================================

void CAimbot::calcFovangleAndVisibility(int ax)
{ 
	PlayerInfo& r = vPlayers[ax];
	r.fovangle    = calcFovAngle(me.pmEyePos, me.viewAngles, r.origin() );
	
	if(r.updateType() == 0 || r.updateType() == 2 || !r.isAlive()) { r.visible = false; return; }

	float fov = 90; //1=10 , 2=30 , 3=90 , 4=360
	

	r.visible = TargetRegion(ax);
	
	if(0) {}
	else if (!CheckTeam(ax))					{ r.canAim = 0; }
	else if (IsShielded(ax))					{ r.canAim = 0; }
	else if (r.fovangle>fov)					{ r.canAim = 0; }	
	else if (bSoonvisible(ax))					{ r.canAim = 1; }
	/*else if (cvar.autowall)
	{
		int damage = GetDamageVec(ax, true);
		if (damage)
		{
			r.canAim = 2;
		}
		else
		{
			damage = GetDamageVec(ax, false);
			if (damage)
				r.canAim = 1;
			else
				r.canAim = 0;
		}
	}*/
	else										{ r.canAim = r.visible; }
}
//==============================================================================

int CAimbot::GetDamageVec(int ax, bool onlyvis)
{
	int hitdamage, penetration = WALL_PEN0;
	vec3_t vecEnd, up, right, forward, EntViewOrg, PlayerOrigin, playerAngles, targetspot;

	VectorCopy(vPlayers[ax].vHitbox,PlayerOrigin);

	playerAngles[0]=0;
	playerAngles[1]=vPlayers[ax].getEnt()->angles[1];
	playerAngles[2]=0;

    gEngfuncs.pfnAngleVectors(playerAngles, forward, right, up);

	forward[2] = -forward[2];

	if (!onlyvis)
		penetration = CorrectGunX();

	targetspot[0] = PlayerOrigin[0] + up[0] * 2.5 + forward[0] * 0.5 + right[0] * 0.0;
	targetspot[1] = PlayerOrigin[1] + up[1] * 2.5 + forward[1] * 0.5 + right[1] * 0.0;
	targetspot[2] = PlayerOrigin[2] + up[2] * 2.5 + forward[2] * 0.5 + right[2] * 0.0;

	hitdamage = CanPenetrate(me.pmEyePos, targetspot, penetration);
	if (hitdamage > 0)
		return hitdamage;

	return 0;
}
//==============================================================================
#define SPIN_REVS_PER_SECOND 6.0f   // adjust to taste 
void CAimbot::FixupAngleDifference(usercmd_t *usercmd) 
{ 
    // thanks tetsuo for this copy/paste 
    cl_entity_t *pLocal; 
    Vector viewforward, viewright, viewup, aimforward, aimright, aimup, vTemp; 
    float newforward, newright, newup, newmagnitude, fTime; 
    float forward = g_Originalcmd.forwardmove; 
    float right = g_Originalcmd.sidemove; 
    float up = g_Originalcmd.upmove; 

    pLocal = gEngfuncs.GetLocalPlayer(); 
    if(!pLocal) 
        return; 

// this branch makes sure your horizontal velocity is not affected when fixing up the movement angles -- it isn't specific to spinning and you can use it with the source tetsuo posted in his forum too 
if(pLocal->curstate.movetype == MOVETYPE_WALK) 
    { 
        gEngfuncs.pfnAngleVectors(Vector(0.0f, g_Originalcmd.viewangles.y, 0.0f), viewforward, viewright, viewup); 
    } 
    else 
    { 
        gEngfuncs.pfnAngleVectors(g_Originalcmd.viewangles, viewforward, viewright, viewup); 
    } 

    // SPIN!!! 
    int iHasShiftHeld = GetAsyncKeyState(VK_LSHIFT); 
    if(pLocal->curstate.movetype == MOVETYPE_WALK && !iHasShiftHeld && !(usercmd->buttons & IN_ATTACK) && !(usercmd->buttons & IN_USE)) 
    { 
        fTime = gEngfuncs.GetClientTime(); 
        usercmd->viewangles.y = fmod(fTime * SPIN_REVS_PER_SECOND * 360.0f, 360.0f); 
    } 

// this branch makes sure your horizontal velocity is not affected when fixing up the movement angles -- it isn't specific to spinning and you can use it with the source tetsuo posted in his forum too 
    if(pLocal->curstate.movetype == MOVETYPE_WALK) 
    { 
        gEngfuncs.pfnAngleVectors(Vector(0.0f, usercmd->viewangles.y, 0.0f), aimforward, aimright, aimup); 
    } 
    else 
    { 
        gEngfuncs.pfnAngleVectors(usercmd->viewangles, aimforward, aimright, aimup); 
    } 

        newforward = DotProduct(forward * viewforward.Normalize(), aimforward) + DotProduct(right * viewright.Normalize(), aimforward) + DotProduct(up * viewup.Normalize(), aimforward); 
        newright = DotProduct(forward * viewforward.Normalize(), aimright) + DotProduct(right * viewright.Normalize(), aimright) + DotProduct(up * viewup.Normalize(), aimright); 
        newup = DotProduct(forward * viewforward.Normalize(), aimup) + DotProduct(right * viewright.Normalize(), aimup) + DotProduct(up * viewup.Normalize(), aimup); 

    usercmd->forwardmove = newforward; 
    usercmd->sidemove = newright; 
    usercmd->upmove = newup; 
} 
