CON_COMMAND_CHAT(silence, "silenced a player")
{
	int iCommandPlayer = -1;

	if (player)
	{
		iCommandPlayer = player->GetPlayerSlot();

		ZEPlayer *pPlayer = g_playerManager->GetPlayer(iCommandPlayer);

		if (!pPlayer->IsAdminFlagSet(ADMFLAG_BAN))
		{
			ClientPrint(player, HUD_PRINTTALK, CHAT_PREFIX "You don't have access to this command.");
			return;
		}
	}

	if (args.ArgC() < 3)
	{
		ClientPrint(player, HUD_PRINTTALK, CHAT_PREFIX "Usage: !silence <name> <duration/0 (permanent)>");
		return;
	}

	int iNumClients = 0;
	int pSlot[MAXPLAYERS];

	ETargetType nType = g_playerManager->TargetPlayerString(iCommandPlayer, args[1], iNumClients, pSlot);

	if (!iNumClients)
	{
		ClientPrint(player, HUD_PRINTTALK, CHAT_PREFIX "Target not found.");
		return;
	}

	int iDuration = V_StringToInt32(args[2], -1);

	if (iDuration < 0)
	{
		ClientPrint(player, HUD_PRINTTALK, CHAT_PREFIX "Invalid duration.");
		return;
	}

	if (iDuration == 0 && nType >= ETargetType::ALL)
	{
		ClientPrint(player, HUD_PRINTTALK, CHAT_PREFIX "You may only permanently silenced individuals.");
		return;
	}

	const char *pszCommandPlayerName = player ? player->GetPlayerName() : "Console";

	char szAction[64];
	V_snprintf(szAction, sizeof(szAction), " for %i minutes", iDuration);

	for (int i = 0; i < iNumClients; i++)
	{
		CBasePlayerController* pTarget = (CBasePlayerController*)g_pEntitySystem->GetBaseEntity((CEntityIndex)(pSlot[i] + 1));

		if (!pTarget)
			continue;

		ZEPlayer* pTargetPlayer = g_playerManager->GetPlayer(pSlot[i]);

		if (pTargetPlayer->IsFakeClient())
			continue;

		CInfractionBase* infraction = new CMuteInfraction(iDuration, pTargetPlayer->GetSteamId64());

		// We're overwriting the infraction, so remove the previous one first
		g_pAdminSystem->FindAndRemoveInfraction(pTargetPlayer, CInfractionBase::Mute);
		g_pAdminSystem->AddInfraction(infraction);
		infraction->ApplyInfraction(pTargetPlayer);
		g_pAdminSystem->SaveInfractions();

		if (iDuration > 0)
			PrintSingleAdminAction(pszCommandPlayerName, pTarget->GetPlayerName(), "silenced", szAction);
		else
			PrintSingleAdminAction(pszCommandPlayerName, pTarget->GetPlayerName(), "permanently silenced");
	}

	g_pAdminSystem->SaveInfractions();

	for (int i = 0; i < iNumClients; i++)
	{
		CBasePlayerController *pTarget = (CBasePlayerController *)g_pEntitySystem->GetBaseEntity((CEntityIndex)(pSlot[i] + 1));

		if (!pTarget)
			continue;

		ZEPlayer *pTargetPlayer = g_playerManager->GetPlayer(pSlot[i]);

		if (pTargetPlayer->IsFakeClient())
			continue;

		CInfractionBase *infraction = new CGagInfraction(iDuration, pTargetPlayer->GetSteamId64());

		// We're overwriting the infraction, so remove the previous one first
		g_pAdminSystem->FindAndRemoveInfraction(pTargetPlayer, CInfractionBase::Gag);
		g_pAdminSystem->AddInfraction(infraction);
		infraction->ApplyInfraction(pTargetPlayer);
	}

	g_pAdminSystem->SaveInfractions();

	PrintMultiAdminAction(nType, pszCommandPlayerName, "silenced", szAction);
}
//******************END OF SILECE*****UNSILENCE*******************************************************************
CON_COMMAND_CHAT(unsilence, "unsilenced a player")
{
	int iCommandPlayer = -1;

	if (player)
	{
		iCommandPlayer = player->GetPlayerSlot();

		ZEPlayer *pPlayer = g_playerManager->GetPlayer(iCommandPlayer);

		if (!pPlayer->IsAdminFlagSet(ADMFLAG_UNBAN))
		{
			ClientPrint(player, HUD_PRINTTALK, CHAT_PREFIX "You don't have access to this command.");
			return;
		}
	}

	if (args.ArgC() < 2)
	{
		ClientPrint(player, HUD_PRINTTALK, CHAT_PREFIX "Usage: !unsilence <name>");
		return;
	}

	int iNumClients = 0;
	int pSlot[MAXPLAYERS];

	ETargetType nType = g_playerManager->TargetPlayerString(iCommandPlayer, args[1], iNumClients, pSlot);

	if (!iNumClients)
	{
		ClientPrint(player, HUD_PRINTTALK, CHAT_PREFIX "Target not found.");
		return;
	}

	const char *pszCommandPlayerName = player ? player->GetPlayerName() : "Console";

	for (int i = 0; i < iNumClients; i++)
	{
		CBasePlayerController *pTarget = (CBasePlayerController *)g_pEntitySystem->GetBaseEntity((CEntityIndex)(pSlot[i] + 1));

		if (!pTarget)
			continue;

		ZEPlayer *pTargetPlayer = g_playerManager->GetPlayer(pSlot[i]);

		if (pTargetPlayer->IsFakeClient())
			continue;

		if (!g_pAdminSystem->FindAndRemoveInfraction(pTargetPlayer, CInfractionBase::Mute) && nType < ETargetType::ALL)
		{
			ClientPrint(player, HUD_PRINTTALK, CHAT_PREFIX "%s is not muted.", pTarget->GetPlayerName());
			continue;
		}

		if (nType < ETargetType::ALL)
			PrintSingleAdminAction(pszCommandPlayerName, pTarget->GetPlayerName(), "unmuted");
	}

	g_pAdminSystem->SaveInfractions();

	PrintMultiAdminAction(nType, pszCommandPlayerName, "unmuted");
		for (int i = 0; i < iNumClients; i++)
	{
		CBasePlayerController *pTarget = (CBasePlayerController *)g_pEntitySystem->GetBaseEntity((CEntityIndex)(pSlot[i] + 1));

		if (!pTarget)
			continue;

		ZEPlayer *pTargetPlayer = g_playerManager->GetPlayer(pSlot[i]);

		if (pTargetPlayer->IsFakeClient())
			continue;

		if (!g_pAdminSystem->FindAndRemoveInfraction(pTargetPlayer, CInfractionBase::Gag) && nType < ETargetType::ALL)
		{
			ClientPrint(player, HUD_PRINTTALK, CHAT_PREFIX "%s is not gagged.", pTarget->GetPlayerName());
			continue;
		}

		if (nType < ETargetType::ALL)
			PrintSingleAdminAction(pszCommandPlayerName, pTarget->GetPlayerName(), "ungagged");
	}

	g_pAdminSystem->SaveInfractions();

	PrintMultiAdminAction(nType, pszCommandPlayerName, "ungagged");
	PrintMultiAdminAction(nType, pszCommandPlayerName, "unsilenced");
}

//**************************************END UNSILENCE*****************************************************
//****************************************************MOVE******************************************************
bool caseInsensitiveStringCompare( const std::string& str1, const std::string& str2 ) 
{
    std::string str1Cpy( str1 );
    std::string str2Cpy( str2 );
    std::transform( str1Cpy.begin(), str1Cpy.end(), str1Cpy.begin(), ::tolower );
    std::transform( str2Cpy.begin(), str2Cpy.end(), str2Cpy.begin(), ::tolower );
    return ( str1Cpy == str2Cpy );
}
CON_COMMAND_CHAT(move, "set a player's team")
{
	int iCommandPlayer = -1;

	if (player)
	{
		iCommandPlayer = player->GetPlayerSlot();

		ZEPlayer *pPlayer = g_playerManager->GetPlayer(player->GetPlayerSlot());

		if (!pPlayer->IsAdminFlagSet(ADMFLAG_SLAY))
		{
			ClientPrint(player, HUD_PRINTTALK, CHAT_PREFIX "You don't have access to this command.");
			return;
		}

		if (args.ArgC() < 3)
		{
			ClientPrint(player, HUD_PRINTTALK, CHAT_PREFIX" Usage: !move <name> <team (t,ct,spec)>");
			return;
		}
	}

	int iNumClients = 0;
	int pSlots[MAXPLAYERS];

	ETargetType nType = g_playerManager->TargetPlayerString(iCommandPlayer, args[1], iNumClients, pSlots);

	if (!iNumClients)
	{
		ClientPrint(player, HUD_PRINTTALK, CHAT_PREFIX "Target not found.");
		return;
	}
	int iTeam = -1;

	if ( caseInsensitiveStringCompare(args[2], "T" )) {
    iTeam = 2;
  
} else if ( caseInsensitiveStringCompare(args[2], "CT" )) {
    iTeam = 3;
   
} else if ( caseInsensitiveStringCompare(args[2], "SPEC" )) {
    iTeam = 1;
   //strcpy(cTeam, "SPEC");
}

	// int iTeam = V_StringToInt32(args[2], -1);

	if (iTeam < CS_TEAM_NONE || iTeam > CS_TEAM_CT)
	{
		ClientPrint(player, HUD_PRINTTALK, CHAT_PREFIX "Invalid team specified, use t,ct,spec");
		return;
	}

	const char *pszCommandPlayerName = player ? player->GetPlayerName() : "Console";

	constexpr const char *teams[] = {"none", "spectators", "terrorists", "counter-terrorists"};

	char szAction[64];
	V_snprintf(szAction, sizeof(szAction), " to %s.", teams[iTeam]);

	for (int i = 0; i < iNumClients; i++)
	{
		CCSPlayerController *pTarget = (CCSPlayerController *)g_pEntitySystem->GetBaseEntity((CEntityIndex)(pSlots[i] + 1));

		if (!pTarget)
			continue;

		addresses::CCSPlayerController_SwitchTeam(pTarget, iTeam);

		if (nType < ETargetType::ALL)
			PrintSingleAdminAction(pszCommandPlayerName, pTarget->GetPlayerName(), "moved", szAction);
			//*************************slay********************
		pTarget->GetPawn()->CommitSuicide(false, true);
	}

	PrintMultiAdminAction(nType, pszCommandPlayerName, "moved", szAction);
}
