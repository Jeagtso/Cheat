#include "main.h"

CAimbot* pAimbot;

void CAimbot::Update()
{
	int iMode = TheCamera.m_aCams[0].m_nMode;
	bCrosshair = iMode == 53 || iMode == 7;
	

	if (pSAMP->getPlayers()->pLocalPlayer->byteCurrentWeapon != 34)
	{
		vecCrosshair.fX = ImGui::GetIO().DisplaySize.x * 0.5299999714f;
		vecCrosshair.fY = ImGui::GetIO().DisplaySize.y * 0.4f;
	}
	else
	{
		vecCrosshair.fX = ImGui::GetIO().DisplaySize.x / 2.f;
		vecCrosshair.fY = ImGui::GetIO().DisplaySize.y / 2.f;
	}

	
	
	GetAimingPlayer2();
	Triggerbot();

	GetAimingPlayer();

	SmoothAimbot();
	ProAimbot();
	
}


void CAimbot::Render()
{
	if (bCrosshair && g_Config.g_Aimbot.bAimbot && g_Config.g_Aimbot.bAimbotEnabled[pSAMP->getPlayers()->pLocalPlayer->byteCurrentWeapon])
	{
		if (g_Config.g_Aimbot.bDrawRange)
			g_Config.g_Aimbot.iRangeStyle ? pRender->DrawCircle(vecCrosshair, (float)g_Config.g_Aimbot.iAimbotConfig[pSAMP->getPlayers()->pLocalPlayer->byteCurrentWeapon][RANGE], g_Config.g_Aimbot.colorRange, g_Config.g_Aimbot.fOutlineThickness)
			: pRender->DrawCircleFilled(vecCrosshair, (float)g_Config.g_Aimbot.iAimbotConfig[pSAMP->getPlayers()->pLocalPlayer->byteCurrentWeapon][RANGE], g_Config.g_Aimbot.colorRange);

		if (g_Config.g_Aimbot.bDrawTracer && iTargetPlayer != -1)
			pRender->DrawLine(vecCrosshair, vecTargetBone, { 0.f, 1.f, 0.f }, g_Config.g_Aimbot.fOutlineThickness);
	}
}

void CAimbot::GetAimingPlayer()
{
	iTargetPlayer = -1;
	float fNearestDistance = 9999.f;

	if (g_Config.g_Aimbot.bAimbot || g_Config.g_Aimbot.bSmooth || g_Config.g_Aimbot.bSilent || g_Config.g_Aimbot.bProAim)
	{
		CVector vecCamera, vecOrigin, vecTarget;
		Utils::getBonePosition(FindPlayerPed(), BONE_RIGHTWRIST, &vecOrigin);
		TheCamera.Find3rdPersonCamTargetVector(100.f, vecOrigin, &vecCamera, &vecTarget);

		for (int i = 0; i < SAMP_MAX_PLAYERS; i++)
		{
			if (!pSAMP->isPlayerStreamed(i))
				continue;

			CPed* pPed = CPools::GetPed(pSAMP->getPlayers()->pRemotePlayer[i]->pPlayerData->pSAMP_Actor->ulGTAEntityHandle);
			if (!pPed || !pPed->IsAlive())
				continue;

			if (g_Config.g_Aimbot.bTeamProtect && pSAMP->getPlayerColor(i) == pSAMP->getPlayerColor(pSAMP->getPlayers()->sLocalPlayerID))
				continue;

			if (!g_Config.g_Aimbot.bIgnoreEverything)
			{
				if (g_Config.g_Aimbot.bIgnoreAFK && pSAMP->getPlayers()->pRemotePlayer[i]->pPlayerData->iAFKState == 2)
					continue;

				float fTargetDistance = Math::vect3_dist(&pSAMP->getPlayers()->pRemotePlayer[i]->pPlayerData->pSAMP_Actor->pGTA_Ped->base.matrix[12], &pSAMP->getPlayers()->pLocalPlayer->pSAMP_Actor->pGTA_Ped->base.matrix[12]);
				if (!g_Config.g_Aimbot.bIgnoreMaxDistance && fTargetDistance > fWeaponRange[pSAMP->getPlayers()->pLocalPlayer->byteCurrentWeapon])
					continue;
			}

			for (auto iBone : iBoneList)
			{
				CVector vecBone, vecBoneScreen;
				Utils::getBonePosition(pPed, (ePedBones)iBone, &vecBone);
				Utils::CalcScreenCoors(&vecBone, &vecBoneScreen);
				if (vecBoneScreen.fZ < 1.0f)
					continue;

				float fCentreDistance = Math::vect2_dist(&vecCrosshair, &vecBoneScreen);
				if (g_Config.g_Aimbot.bAimbot && fCentreDistance >= (float)g_Config.g_Aimbot.iAimbotConfig[pSAMP->getPlayers()->pLocalPlayer->byteCurrentWeapon][RANGE] * 1.5f)
					continue;

				if (!g_Config.g_Aimbot.bIgnoreEverything && !g_Config.g_Aimbot.bLockThroughObjects && !CWorld::GetIsLineOfSightClear(vecCamera, vecBone, true, true, false, true, true, true, false))
					continue;

				if (fCentreDistance <= fNearestDistance)
				{
					fNearestDistance = fCentreDistance;
					iTargetPlayer = i; 
					iTargetBone = iBone;
					vecTargetBone = vecBoneScreen;
					break;
				}
			}
		}
	}
}
void CAimbot::GetAimingPlayer2()
{
	iTargetPlayer2 = -1;
	float fNearestDistance2 = 9999.f;

	if (g_Config.g_Aimbot.bAimbot || g_Config.g_Aimbot.bSmooth || g_Config.g_Aimbot.bSilent || g_Config.g_Aimbot.bProAim)
	{
		CVector vecCamera, vecOrigin, vecTarget;
		Utils::getBonePosition(FindPlayerPed(), BONE_RIGHTWRIST, &vecOrigin);
		TheCamera.Find3rdPersonCamTargetVector(100.f, vecOrigin, &vecCamera, &vecTarget);

		for (int i = 0; i < SAMP_MAX_PLAYERS; i++)
		{
			if (!pSAMP->isPlayerStreamed(i))
				continue;

			CPed* pPed = CPools::GetPed(pSAMP->getPlayers()->pRemotePlayer[i]->pPlayerData->pSAMP_Actor->ulGTAEntityHandle);
			if (!pPed || !pPed->IsAlive())
				continue;

			if (g_Config.g_Aimbot.bTeamProtect && pSAMP->getPlayerColor(i) == pSAMP->getPlayerColor(pSAMP->getPlayers()->sLocalPlayerID))
				continue;

			if (!g_Config.g_Aimbot.bIgnoreEverything)
			{
				if (g_Config.g_Aimbot.bIgnoreAFK && pSAMP->getPlayers()->pRemotePlayer[i]->pPlayerData->iAFKState == 2)
					continue;

				float fTargetDistance = Math::vect3_dist(&pSAMP->getPlayers()->pRemotePlayer[i]->pPlayerData->pSAMP_Actor->pGTA_Ped->base.matrix[12], &pSAMP->getPlayers()->pLocalPlayer->pSAMP_Actor->pGTA_Ped->base.matrix[12]);
				if (!g_Config.g_Aimbot.bIgnoreMaxDistance && fTargetDistance > fWeaponRange[pSAMP->getPlayers()->pLocalPlayer->byteCurrentWeapon])
					continue;
			}

			for (auto iBone2 : iBoneList)
			{
				CVector vecBone, vecBoneScreen;
				Utils::getBonePosition2(pPed, (ePedBones)iBone2, &vecBone);
				Utils::CalcScreenCoors2(&vecBone, &vecBoneScreen);
				if (vecBoneScreen.fZ < 1.0f)
					continue;

				float fCentreDistance = Math::vect2_dist(&vecCrosshair, &vecBoneScreen);
				if (g_Config.g_Aimbot.bAimbot && fCentreDistance >= (float)g_Config.g_Aimbot.iAimbotConfig[pSAMP->getPlayers()->pLocalPlayer->byteCurrentWeapon][SRANGE] * 1.5f)
					continue;

				if (!g_Config.g_Aimbot.bIgnoreEverything && !g_Config.g_Aimbot.bLockThroughObjects && !CWorld::GetIsLineOfSightClear(vecCamera, vecBone, true, true, false, true, true, true, false))
					continue;

				if (fCentreDistance <= fNearestDistance2)
				{
					fNearestDistance2 = fCentreDistance;
					iTargetPlayer2 = i;
					iTargetBone2 = iBone2;
					vecTargetBone2 = vecBoneScreen;
					break;
				}
			}
		}
	}
}

bool __stdcall CAimbot::hkFireInstantHit(void* this_, CEntity* pFiringEntity, CVector* pOrigin, CVector* pMuzzle, CEntity* pTargetEntity, CVector* pTarget, CVector* pVec, bool bCrossHairGun, bool bCreateGunFx)
{
	if (pFiringEntity == (CEntity*)FindPlayerPed() && g_Config.g_Aimbot.bSilent && pAimbot->iTargetPlayer2 != -1 && g_Config.g_Aimbot.bAimbotEnabled[pSAMP->getPlayers()->pLocalPlayer->byteCurrentWeapon] && rand() % 100 <= g_Config.g_Aimbot.iAimbotConfig[pSAMP->getPlayers()->pLocalPlayer->byteCurrentWeapon][SILENT])
	{
		if (!isKeyDown(VK_INSERT) && isKeyDown(VK_LBUTTON) && g_Config.g_Aimbot.bGTS && g_Config.g_Aimbot.bTriggerbot && g_Config.g_Aimbot.bAimbot)
		{
			Memory::memcpy_safe((void*)0x740B4E, "\x6A\x01\x6A\x01", 4);
			*reinterpret_cast<float*>(0x8D6114) = 5.f;
		}
		else {
			CPed* pPed = CPools::GetPed(pSAMP->getPlayers()->pRemotePlayer[pAimbot->iTargetPlayer2]->pPlayerData->pSAMP_Actor->ulGTAEntityHandle);
			if (pPed)
			{
				CVector vecCamera, vecOrigin, vecTarget;
				Utils::getBonePosition2(FindPlayerPed(), BONE_RIGHTWRIST, &vecOrigin);
				TheCamera.Find3rdPersonCamTargetVector(100.f, vecOrigin, &vecCamera, &vecTarget);
				if (pAimbot->bCrosshair)
					pOrigin = &vecCamera;

				CVector vecBone;
				Utils::getBonePosition2(pPed, (ePedBones)pAimbot->iTargetBone2, &vecBone);
				pTarget = &vecBone;

				pSecure->memcpy_safe((void*)0x740B4E, g_Config.g_Aimbot.bLockThroughObjects || g_Config.g_Aimbot.bIgnoreEverything ? "\x6A\x00\x6A\x00" : "\x6A\x01\x6A\x01", 4);
				pSecure->Write(0x8D6114, (float)(g_Config.g_Aimbot.fSilentAccuracy / 20.f));
			}
		}
	}
	else
	{
		Memory::memcpy_safe((void*)0x740B4E, "\x6A\x01\x6A\x01", 4);
		*reinterpret_cast<float*>(0x8D6114) = 5.f;
	}
	return pAimbot->oFireInstantHit(this_, pFiringEntity, pOrigin, pMuzzle, pTargetEntity, pTarget, pVec, bCrossHairGun, bCreateGunFx);
}

bool __cdecl CAimbot::hkAddBullet(CEntity* pCreator, eWeaponType weaponType, CVector vecPosition, CVector vecVelocity)
{
	if (pCreator == FindPlayerPed() && g_Config.g_Aimbot.bSilent && pAimbot->iTargetPlayer2 != -1 && g_Config.g_Aimbot.bAimbotEnabled[34] && rand() % 100 <= g_Config.g_Aimbot.iAimbotConfig[34][SILENT])
	{
		CPed* pPed = CPools::GetPed(pSAMP->getPlayers()->pRemotePlayer[pAimbot->iTargetPlayer2]->pPlayerData->pSAMP_Actor->ulGTAEntityHandle);
		if (pPed)
		{
			CVector vecBone;
			Utils::getBonePosition2(pPed, (ePedBones)pAimbot->iTargetBone2, &vecBone);
			//Utils::getBonePosition2(pPed, (ePedBones)pAimbot->iTargetBone, &vecBone);
			vecVelocity = vecBone - vecPosition;

			pSecure->memcpy_safe((void*)0x736212, g_Config.g_Aimbot.bLockThroughObjects || g_Config.g_Aimbot.bIgnoreEverything ? "\x6A\x00\x6A\x00" : "\x6A\x01\x6A\x01", 4);
		}
	}
	else Memory::memcpy_safe((void*)0x736212, "\x6A\x01\x6A\x01", 4);

	return pAimbot->oAddBullet(pCreator, weaponType, vecPosition, vecVelocity);
}

float __cdecl CAimbot::hkTargetWeaponRangeMultiplier(CEntity* pVictim, CEntity* pOwner)
{
	if (pOwner == FindPlayerPed() && (g_Config.g_Aimbot.bIgnoreMaxDistance || g_Config.g_Aimbot.bIgnoreEverything))
		return 100.f;

	return pAimbot->oTargetWeaponRangeMultiplier(pVictim, pOwner);
}

void CAimbot::SmoothAimbot()
{
	if (g_Config.g_Aimbot.bSmooth && g_Config.g_Aimbot.bAimbotEnabled[pSAMP->getPlayers()->pLocalPlayer->byteCurrentWeapon] && iTargetPlayer != -1 && bCrosshair)
	{
		if ((g_Config.g_Aimbot.bSmoothIsFire && !isKeyDown(VK_LBUTTON)) || ((g_Config.g_Aimbot.bGTS && !isKeyDown(VK_PAUSE)) /*&& tik < (GetTickCount64() - 100)*/))
			return;

		/*if (((g_Config.g_Aimbot.tikEnd - g_Config.g_Aimbot.tikStart) > 100) && g_Config.g_Aimbot.flg == 0)
		{
			g_Config.g_Aimbot.flg = -1;
			return;
		}*/

		if (g_Config.g_Aimbot.flg == -1)
		{
			g_Config.g_Aimbot.tikStart = GetTickCount64();
			if (g_Config.g_Aimbot.tikEnd > (GetTickCount64() - 1000))
				return;
		}

		CPed* pPed = CPools::GetPed(pSAMP->getPlayers()->pRemotePlayer[pAimbot->iTargetPlayer]->pPlayerData->pSAMP_Actor->ulGTAEntityHandle);
		if (!pPed)
			return;

		if (g_Config.g_Aimbot.bStopOnBody || g_Config.g_Aimbot.bGTS)
		{
			CVector vecCamera, vecOrigin, vecTarget;
			if (pSAMP->getPlayers()->pLocalPlayer->byteCurrentWeapon != 34)
			{
				Utils::getBonePosition(FindPlayerPed(), BONE_RIGHTWRIST, &vecOrigin);
				TheCamera.Find3rdPersonCamTargetVector(100.f, vecOrigin, &vecCamera, &vecTarget);
			}
			else
			{
				CVector vecFront = TheCamera.m_aCams[0].m_vecFront;
				vecFront.Normalise();
				vecCamera = *TheCamera.GetGameCamPosition();
				vecCamera += (vecFront * 2.0f);
				vecTarget = vecCamera + (vecFront * 100.f);
			}
			CColPoint pCollision; CEntity* pCollisionEntity = NULL;
			bool bCollision = CWorld::ProcessLineOfSight(vecCamera, vecTarget, pCollision, pCollisionEntity, !g_Config.g_Aimbot.bLockThroughObjects, !g_Config.g_Aimbot.bLockThroughObjects, true, true, true, true, false, true);
			if (bCollision && pCollisionEntity && pCollisionEntity->m_nType == ENTITY_TYPE_PED && pCollisionEntity == (CEntity*)pPed && g_Config.g_Aimbot.flg == 0 && ((g_Config.g_Aimbot.tikEnd - g_Config.g_Aimbot.tikStart) > 50))
			{
				g_Config.g_Aimbot.flg = -1;
				return;
			}
		}

		CVector vecOrigin = *TheCamera.GetGameCamPosition(), vecTarget = *Utils::getBonePosition(pPed, (ePedBones)BONE_PELVIS1, &vecTarget), vecVector = vecOrigin - vecTarget;

		float fFix = 0.f, fVecX = 0.f, fZ = 0.f, fX = 0.f;

		if (TheCamera.m_aCams[0].m_nMode == 53 || TheCamera.m_aCams[0].m_nMode == 55)
		{
			float fMult = tan(TheCamera.FindCamFOV() / 2.0f * 0.017453292f);
			fZ = M_PI - atan2(1.0f, fMult * ((1.0f - CCamera::m_f3rdPersonCHairMultY * 2.0f) * (1.0f / CDraw::ms_fAspectRatio)));
			fX = M_PI - atan2(1.0f, fMult * (CCamera::m_f3rdPersonCHairMultX * 2.0f - 1.0f));
		}
		else fX = fZ = M_PI / 2;

		BYTE byteWeapon = pSAMP->getPlayers()->pLocalPlayer->byteCurrentWeapon;
		if (byteWeapon >= 22 && byteWeapon <= 29 || byteWeapon == 32) fFix = 0.04253f;
		else if (byteWeapon == 30 || byteWeapon == 31) fFix = 0.028f;
		else if (byteWeapon == 33) fFix = 0.01897f;

		float fDistX = sqrt(vecVector.fX * vecVector.fX + vecVector.fY * vecVector.fY);

		if (vecVector.fX <= 0.0 && vecVector.fY >= 0.0 || vecVector.fX >= 0.0 && vecVector.fY >= 0.0)
			fVecX = (acosf(vecVector.fX / fDistX) + fFix) - TheCamera.m_aCams[0].m_fHorizontalAngle;
		if (vecVector.fX >= 0.0 && vecVector.fY <= 0.0 || vecVector.fX <= 0.0 && vecVector.fY <= 0.0)
			fVecX = (-acosf(vecVector.fX / fDistX) + fFix) - TheCamera.m_aCams[0].m_fHorizontalAngle;

		float fSmoothX = fVecX / (g_Config.g_Aimbot.iAimbotConfig[byteWeapon][SMOOTH] * 2);

		if (fSmoothX > -1.0 && fSmoothX < 0.5 && fVecX > -2.0 && fVecX < 2.0)
			TheCamera.m_aCams[0].m_fHorizontalAngle += fSmoothX;

		if (g_Config.g_Aimbot.bSmoothLockY)
		{
			float fDistZ = sqrt(vecVector.fX * vecVector.fX + vecVector.fY * vecVector.fY);
			float fSmoothZ = (atan2f(fDistZ, vecVector.fZ) - fZ - TheCamera.m_aCams[0].m_fVerticalAngle) / (g_Config.g_Aimbot.iAimbotConfig[byteWeapon][SMOOTH] * 2);
			TheCamera.m_aCams[0].m_fVerticalAngle += fSmoothZ;
		}

		g_Config.g_Aimbot.tikEnd = GetTickCount64();
		g_Config.g_Aimbot.flg = 0;
	}
}

void CAimbot::ProAimbot()
{
	if (g_Config.g_Aimbot.bProAim && g_Config.g_Aimbot.bAimbotEnabled[pSAMP->getPlayers()->pLocalPlayer->byteCurrentWeapon] && iTargetPlayer != -1 && bCrosshair && isKeyDown(VK_LBUTTON))
	{
		CPed* pPed = CPools::GetPed(pSAMP->getPlayers()->pRemotePlayer[pAimbot->iTargetPlayer]->pPlayerData->pSAMP_Actor->ulGTAEntityHandle);
		if (!pPed)
			return;

		CVector vecCamera, vecOrigin, vecTarget;
		if (pSAMP->getPlayers()->pLocalPlayer->byteCurrentWeapon != 34)
		{
			Utils::getBonePosition(FindPlayerPed(), BONE_RIGHTWRIST, &vecOrigin);
			TheCamera.Find3rdPersonCamTargetVector(2.f, vecOrigin, &vecCamera, &vecTarget);
		}
		else
		{
			CVector vecFront = TheCamera.m_aCams[0].m_vecFront;
			vecFront.Normalise();
			vecCamera = *TheCamera.GetGameCamPosition();
			vecCamera += (vecFront * 2.0f);
			vecTarget = vecCamera + (vecFront * 1.5f);
		}

		pPed->SetPosn(vecTarget);
	}
}
void CAimbot::Triggerbot()
{
	if (g_Config.g_Aimbot.tBot_Release == true && g_Config.g_Aimbot.tBot_LastTick != -1 && g_Config.g_Aimbot.tBot_LastTick < (GetTickCount64() - 100))  // check if release variable is true and the last tick is not -1 and 50 ms has passed
	{

		ZeroMemory(&g_Config.g_Aimbot.Input, sizeof(INPUT)); // set it to zero       
		g_Config.g_Aimbot.Input.type = INPUT_MOUSE;  // input type to mouse
		g_Config.g_Aimbot.Input.mi.dwFlags = MOUSEEVENTF_LEFTUP;  // event to release a.k.a up
		SendInput(1, &g_Config.g_Aimbot.Input, sizeof(INPUT));  // send it
		g_Config.g_Aimbot.tBot_Release = false; // reset our vars
		g_Config.g_Aimbot.tBot_LastTick = -1;

	}
	if (g_Config.g_Aimbot.bTriggerbot && g_Config.g_Aimbot.bAimbotEnabled[pSAMP->getPlayers()->pLocalPlayer->byteCurrentWeapon] && bCrosshair)
	{
		static ULONGLONG iTick = GetTickCount64();

		if (g_Config.g_Aimbot.bTriggerDelay && GetTickCount64() - iTick < (ULONGLONG)g_Config.g_Aimbot.iTriggerDelay)
			return;

		if ((g_Config.g_Aimbot.bTriggerKeyMMB && !GetAsyncKeyState(VK_SPACE)) || (g_Config.g_Aimbot.bGTS /*&& g_Config.g_Aimbot.bSilent*/ && g_Config.g_Aimbot.bAimbot && !GetAsyncKeyState(VK_INSERT)))
			return;


		if (g_Config.g_Aimbot.bTriggerSync || (g_Config.g_Aimbot.bGTS /*&& g_Config.g_Aimbot.bSilent*/ && g_Config.g_Aimbot.bAimbot && (pSAMP->getPlayers()->pLocalPlayer->byteCurrentWeapon == 34 || pSAMP->getPlayers()->pLocalPlayer->byteCurrentWeapon == 33)))
		{
			if (g_Config.g_Aimbot.bSilent && g_Config.g_Aimbot.bAimbot && iTargetPlayer2 != -1)
			{
				//pKeyHook->g_GameKeyState[BUTTON_CIRCLE] = { 0xFF, true };
				if (g_Config.g_Aimbot.tBot_LastTick2 < (GetTickCount64() - (ULONGLONG)g_Config.g_Aimbot.iTriggerDelay)) {

					g_Config.g_Aimbot.Input.type = INPUT_MOUSE;  // input type is mouse as always
					g_Config.g_Aimbot.Input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;  // hold the key a.k.a down
					SendInput(1, &g_Config.g_Aimbot.Input, sizeof(INPUT));  // send it
					g_Config.g_Aimbot.tBot_LastTick = GetTickCount64(); // get current tick count and set it at tBot_LastTick
					g_Config.g_Aimbot.tBot_Release = true; // set the state to release key

					g_Config.g_Aimbot.tBot_LastTick2 = GetTickCount64();
				}
				iTick = GetTickCount64();
			}
		}
		else
		{
			CVector vecCamera, vecOrigin, vecTarget;
			if (pSAMP->getPlayers()->pLocalPlayer->byteCurrentWeapon != 34)
			{
				Utils::getBonePosition(FindPlayerPed(), BONE_RIGHTWRIST, &vecOrigin);
				TheCamera.Find3rdPersonCamTargetVector(100.f, vecOrigin, &vecCamera, &vecTarget);
			}
			else
			{
				CVector vecFront = TheCamera.m_aCams[0].m_vecFront;
				vecFront.Normalise();
				vecCamera = *TheCamera.GetGameCamPosition();
				vecCamera += (vecFront * 2.0f);
				vecTarget = vecCamera + (vecFront * 100.f);
			}

			CColPoint pCollision; CEntity* pCollisionEntity = NULL;
			bool bCollision = CWorld::ProcessLineOfSight(vecCamera, vecTarget, pCollision, pCollisionEntity, !g_Config.g_Aimbot.bLockThroughObjects, !g_Config.g_Aimbot.bLockThroughObjects, true, true, true, true, false, true);
			if (bCollision && pCollisionEntity && pCollisionEntity->m_nType == ENTITY_TYPE_PED)
			{
				for (int i = 0; i < SAMP_MAX_PLAYERS; i++)
				{
					if (!pSAMP->isPlayerStreamed(i))
						continue;

					CPed* pPed = CPools::GetPed(pSAMP->getPlayers()->pRemotePlayer[i]->pPlayerData->pSAMP_Actor->ulGTAEntityHandle);
					if (!pPed || pPed == FindPlayerPed())
						continue;

					if (pCollisionEntity != pPed)
						continue;

					if (!pPed->IsAlive())
						continue;

					if (g_Config.g_Aimbot.bTeamProtect && pSAMP->getPlayerColor(i) == pSAMP->getPlayerColor(pSAMP->getPlayers()->sLocalPlayerID))
						continue;

					if (!g_Config.g_Aimbot.bIgnoreEverything)
					{
						if (g_Config.g_Aimbot.bIgnoreAFK && pSAMP->getPlayers()->pRemotePlayer[i]->pPlayerData->iAFKState == 2)
							continue;

						float fDistanceFromEnemy = Math::vect3_dist(&pSAMP->getPlayers()->pRemotePlayer[i]->pPlayerData->pSAMP_Actor->pGTA_Ped->base.matrix[12], &pSAMP->getPlayers()->pLocalPlayer->pSAMP_Actor->pGTA_Ped->base.matrix[12]);
						if (!g_Config.g_Aimbot.bIgnoreMaxDistance && fDistanceFromEnemy > fWeaponRange[pSAMP->getPlayers()->pLocalPlayer->byteCurrentWeapon])
							continue;
					}
					//pKeyHook->g_GameKeyState[BUTTON_CIRCLE] = { 0xFF, true };
					if (g_Config.g_Aimbot.tBot_LastTick2 < (GetTickCount64() - (ULONGLONG)g_Config.g_Aimbot.iTriggerDelay)) {

						g_Config.g_Aimbot.Input.type = INPUT_MOUSE;  // input type is mouse as always
						g_Config.g_Aimbot.Input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;  // hold the key a.k.a down
						SendInput(1, &g_Config.g_Aimbot.Input, sizeof(INPUT));  // send it
						g_Config.g_Aimbot.tBot_LastTick = GetTickCount64(); // get current tick count and set it at tBot_LastTick
						g_Config.g_Aimbot.tBot_Release = true; // set the state to release key

						g_Config.g_Aimbot.tBot_LastTick2 = GetTickCount64();
					}
					iTick = GetTickCount64();
					break;
				}
			}

		}
	}
}

