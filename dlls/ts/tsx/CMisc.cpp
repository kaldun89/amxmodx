/*
 * Copyright (c) 2003-2004 Lukasz Wlasinski
 *
 *    This file is part of TS XMod.
 *
 *    TS XMod is free software; you can redistribute it and/or modify it
 *    under the terms of the GNU General Public License as published by the
 *    Free Software Foundation; either version 2 of the License, or (at
 *    your option) any later version.
 *
 *    TS XMod is distributed in the hope that it will be useful, but
 *    WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with TS XMod; if not, write to the Free Software Foundation,
 *    Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *    In addition, as a special exception, the author gives permission to
 *    link the code of this program with the Half-Life Game Engine ("HL
 *    Engine") and Modified Game Libraries ("MODs") developed by Valve,
 *    L.L.C ("Valve").  You must obey the GNU General Public License in all
 *    respects for all of the code used other than the HL Engine and MODs
 *    from Valve.  If you modify this file, you may extend this exception
 *    to your version of the file, but you are not obligated to do so.  If
 *    you do not wish to do so, delete this exception statement from your
 *    version.
 *
 */

#include "CMisc.h"
#include "tsx.h"

// *****************************************************
// class CPlayer
// *****************************************************

void CPlayer::Disconnect()
{
	if ( ignoreBots(pEdict) || !isModuleActive() ) // ignore if he is bot and bots rank is disabled or module is paused
		return;
	rank->updatePosition( &life );
	ingame = false;
}
void CPlayer::PutInServer()
{
	restartStats();
	ingame = true;

	killingSpree = 0;
	items = 0;
    lastFrag = 0;
    lastKill = 0.0;
	is_specialist = 0;

	//debug
	frags = 0;

	if ( ignoreBots(pEdict) )
		return;

	const char* unique;
	const char* name = STRING(pEdict->v.netname);
	bool isip = false;
	switch((int)tsstats_rank->value) {
	case 1: 
		if ( (unique = GETPLAYERAUTHID(pEdict)) == 0 )
			unique = name; // failed to get authid
		break;
	case 2: 
		unique = ip; 
		isip = true;
		break;
	default: 
		unique = name;
	}
	if ( ( rank = g_rank.findEntryInRank( unique , name , isip) ) == 0 )
		ingame = false;
}
void CPlayer::Connect(const char* ippp)
{
	strcpy(ip,ippp);
	// Strip the port from the ip
	for (size_t i = 0; i < sizeof(ip); i++)
	{
		if (ip[i] == ':')
		{
			ip[i] = '\0';
			break;
		}
	}

}

void CPlayer::restartStats(bool all)
{
	if ( all ) memset(weapons,0,sizeof(weapons));
	memset(weaponsRnd,0,sizeof(weaponsRnd));   //DEC-Weapon (Round) stats
	memset(attackers,0,sizeof(attackers));
	memset(victims,0,sizeof(victims));
	memset(&life,0,sizeof(life));
}

void CPlayer::Init( int pi, edict_t* pe )
{
    pEdict = pe;
    index = pi;
	current = 0;
	clearStats = 0.0f;
	ingame = false;
}

void CPlayer::saveKill(CPlayer* pVictim, int wweapon, int hhs, int ttk)
{

	if ( ignoreBots(pEdict,pVictim->pEdict) )
		return;

	pVictim->attackers[index].name = (char*)weaponData[wweapon].name;
	pVictim->attackers[index].kills++;
	pVictim->attackers[index].hs += hhs;
	pVictim->attackers[index].tks += ttk;
	pVictim->attackers[0].kills++;
	pVictim->attackers[0].hs += hhs;
	pVictim->attackers[0].tks += ttk;
	pVictim->weapons[pVictim->current].deaths++;
	pVictim->weapons[0].deaths++;
	pVictim->life.deaths++;
	
	
	pVictim->weaponsRnd[pVictim->current].deaths++; // DEC-Weapon (round) stats
	pVictim->weaponsRnd[0].deaths++;                   // DEC-Weapon (round) stats
	
	int vi = pVictim->index;
	victims[vi].name = (char*)weaponData[wweapon].name;
	victims[vi].deaths++;
	victims[vi].hs += hhs;
	victims[vi].tks += ttk;
	victims[0].deaths++;
	victims[0].hs += hhs;
	victims[0].tks += ttk;
	
	
	weaponsRnd[wweapon].kills++;                // DEC-Weapon (round) stats
	weaponsRnd[wweapon].hs += hhs;         // DEC-Weapon (round) stats
	weaponsRnd[wweapon].tks += ttk;     // DEC-Weapon (round) stats
	weaponsRnd[0].kills++;                     // DEC-Weapon (round) stats
	weaponsRnd[0].hs += hhs;              // DEC-Weapon (round) stats
	weaponsRnd[0].tks += ttk;          // DEC-Weapon (round) stats
	
	weapons[wweapon].kills++;
	weapons[wweapon].hs += hhs;
	weapons[wweapon].tks += ttk;
	weapons[0].kills++;
	weapons[0].hs += hhs;
	weapons[0].tks += ttk;
	life.kills++;
	life.hs += hhs;
	life.tks += ttk;
}

void CPlayer::saveHit(CPlayer* pVictim, int wweapon, int ddamage, int bbody)
{

	if ( ignoreBots(pEdict,pVictim->pEdict) )
		return;

	pVictim->attackers[index].hits++;
	pVictim->attackers[index].damage += ddamage;
	pVictim->attackers[index].bodyHits[bbody]++;
	pVictim->attackers[0].hits++;
	pVictim->attackers[0].damage += ddamage;
	pVictim->attackers[0].bodyHits[bbody]++;

	int vi = pVictim->index;
	victims[vi].hits++;
	victims[vi].damage += ddamage;
	victims[vi].bodyHits[bbody]++;
	victims[0].hits++;
	victims[0].damage += ddamage;
	victims[0].bodyHits[bbody]++;

	weaponsRnd[wweapon].hits++;              // DEC-Weapon (round) stats
	weaponsRnd[wweapon].damage += ddamage;    // DEC-Weapon (round) stats
	weaponsRnd[wweapon].bodyHits[bbody]++;   // DEC-Weapon (round) stats
	weaponsRnd[0].hits++;                   // DEC-Weapon (round) stats
	weaponsRnd[0].damage += ddamage;         // DEC-Weapon (round) stats
	weaponsRnd[0].bodyHits[bbody]++;        // DEC-Weapon (round) stats

	weapons[wweapon].hits++;
	weapons[wweapon].damage += ddamage;
	weapons[wweapon].bodyHits[bbody]++;
	weapons[0].hits++;
	weapons[0].damage += ddamage;
	weapons[0].bodyHits[bbody]++;

	life.hits++;
	life.damage += ddamage;
	life.bodyHits[bbody]++;
}

void CPlayer::saveShot(int weapon)
{

	if ( ignoreBots(pEdict) )
		return;

	victims[0].shots++;
	weapons[weapon].shots++;
	weapons[0].shots++;
	life.shots++;
	weaponsRnd[weapon].shots++;       // DEC-Weapon (round) stats
	weaponsRnd[0].shots++;            // DEC-Weapon (round) stats
}

void CPlayer::SetOffsetF(int offs, float val)
{
	*((float *)pEdict->pvPrivateData + offs) = val;
}

void CPlayer::SetOffset(int offs, int val)
{
	*((int *)pEdict->pvPrivateData + offs) = val;
}

float CPlayer::GetOffsetF(int offs)
{
	return *((float *)pEdict->pvPrivateData + offs);
}

int CPlayer::GetOffset(int offs)
{
	return *((int *)pEdict->pvPrivateData + offs);
}

float CPlayer::GetTime()
{
	return GetOffsetF(TSX_TIME_OFFSET);
}

void CPlayer::SetMoney(int money)
{
	SetOffset(TSX_MONEY_OFFSET, money);
}

void CPlayer::SetSlots(int slots)
{
	SetOffset(TSX_SLOTS_OFFSET, slots);
}

int CPlayer::GetSlots()
{
	return GetOffset(TSX_SLOTS_OFFSET);
}


