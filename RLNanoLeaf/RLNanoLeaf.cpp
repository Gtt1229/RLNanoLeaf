#include "pch.h"
#include <nlohmann/json.hpp>
#include "RLNanoLeaf.h"
#include "IMGUI/imgui_internal.h"
#include "IMGUI/imgui_searchablecombo.h"
#include "IMGUI/imgui_stdlib.h"
#include <filesystem>
#include <fstream>

BAKKESMOD_PLUGIN(RLNanoLeaf, "write a plugin description here", plugin_version, PLUGINTYPE_FREEPLAY)

std::shared_ptr<CVarManagerWrapper> _globalCvarManager;



void RLNanoLeaf::onLoad()
{
	_globalCvarManager = cvarManager;
	
	//Enable Cvars
	cvarManager->registerCvar("cl_rln_enabled", "1", "Enable RL NanoLeaf Plugin", true, true, 0, true, 1);
	cvarManager->registerCvar("cl_rln_teams_enabled", "1", "Enable NanoLeaf Team Colors", true, true, 0, true, 1);
	cvarManager->registerCvar("cl_rln_demos_enabled", "1", "Enable NanoLeaf Demos", true, true, 0, true, 1);
	cvarManager->registerCvar("cl_rln_goalScored_enabled", "1", "Enable NanoLeaf Goal Scored", true, true, 0, true, 1);
	cvarManager->registerCvar("cl_rln_freeplay_enabled", "1", "Enable NanoLeaf Freeplay", true, true, 0, true, 1);
	cvarManager->registerCvar("cl_rln_mainmenu_enabled", "1", "Enable NanoLeaf Mainemenu", true, true, 0, true, 1);
	cvarManager->registerCvar("cl_rln_overtime_enabled", "1", "Enable NanoLeaf Overtime", true, true, 0, true, 1);
	cvarManager->registerCvar("cl_rln_matchCountdown_enabled", "1", "Enable NanoLeaf Match Countdown", true, true, 0, true, 1);
	cvarManager->registerCvar("cl_rln_exit_enabled", "1", "Enable NanoLeaf Exit", true, true, 0, true, 1);
	cvarManager->registerCvar("cl_rln_exit_off", "0", "Enable NanoLeaf to Turn Off on Exit", true, true, 0, true, 1);
	cvarManager->registerCvar("cl_rln_isReplay", "0", "NanoLeaf Replay boolean", true, true, 0, true, 1);
	//cvarManager->registerCvar("cl_rln_logging", "0", "NanoLeaf Logging boolean", true, true, 0, true, 1);
	cvarManager->registerCvar("cl_rln_teamDemoColor_enabled", "1", "NanoLeaf Demo Colors Based on Team Colors boolean", true, true, 0, true, 1);
	cvarManager->registerCvar("cl_rln_teamGoalColor_enabled", "1", "NanoLeaf Goal Colors Based on Team Colors boolean", true, true, 0, true, 1);

	//NanoLeaf IP Cvar
	cvarManager->registerCvar("cl_rln_nanoLeafIP", "192.168.1.256", "NanoLeaf IP");
	
	//NanoLeaf Token
	cvarManager->registerCvar("cl_rln_nanoLeaftoken", "NanoLeafToken", "NanoLeaf Auth Token");
	cvarManager->registerCvar("cl_rln_panelIDs", "1 2 3 4 5", "NanoLeaf Panel IDs"); //May be used in the future for addressing individual panels


	//Team CVARs
	cvarManager->registerCvar("cl_rln_playersTeam", "2");
	cvarManager->registerCvar("cl_rln_otherTeam", "2");
	cvarManager->registerCvar("cl_rln_myTeamPrimaryRGBColor", "(1, 1, 1, 1)");
	cvarManager->registerCvar("cl_rln_OtherTeamPrimaryRGBColor", "(1, 1, 1, 1)");

	//Color CVARs
	cvarManager->registerCvar("cl_rln_demo_color", "(255, 153, 0, 1)", "Demo Lights Color");
	cvarManager->registerCvar("cl_rln_goalScored_color", "(255, 0, 0, 1)", "Goal Lights Color");
	cvarManager->registerCvar("cl_rln_freeplay_color", "(192, 67, 209, 1)", "Freeplay Lights Color");
	cvarManager->registerCvar("cl_rln_mainmenu_color", "(0, 203, 255, 1)", "Main Menu Lights Color");
	cvarManager->registerCvar("cl_rln_overtime_color", "(0, 203, 255, 1)", "Overtime Lights Color");
	cvarManager->registerCvar("cl_rln_matchCountdown_color", "(255, 153, 0, 1)", "Match Countdown Lights Color");
	cvarManager->registerCvar("cl_rln_exit_color", "(0, 0, 0, 1)", "Exit Lights Color");

	//color brightness slider
	cvarManager->registerCvar("cl_rln_brightness", "100", "Brightness",	true,true,0.0F,true,100.0F);

	//Effects CVARs
	cvarManager->registerCvar("cl_rln_effects_enabled", "0", "Use NanoLeaf effects instead of solid colors", true, true, 0, true, 1);
	cvarManager->registerCvar("cl_rln_freeplay_effect", "effect1", "Freeplayer Effect Name");
	cvarManager->registerCvar("cl_rln_overtime_effect", "effect1", "Overtime Effect Name");
	cvarManager->registerCvar("cl_rln_mainmenu_effect", "effect1", "Main Menu Effect Name");
	cvarManager->registerCvar("cl_rln_exit_effect", "effect1", "Exit Effect Name");




	//NanoLeaf data files
	this->dataDir = this->gameWrapper->GetDataFolder().append("RLNanoLeaf");
	this->mainPath = std::filesystem::path(dataDir).append(this->mainFile);
	this->tmpPath = std::filesystem::path(dataDir).append(this->tmpFile);
	this->bakPath = std::filesystem::path(dataDir).append(this->bakFile);

	this->cvarManager->registerNotifier("rlnanoleaf_reload", [this](const std::vector<std::string>& commands) {
		LoadData();
		}, "Reloads the json data from file", PERMISSION_ALL);


	//Log plugin started
	this->Log("RL NanoLeaf has started");

	//call LoadHooks method
	this->LoadHooks();
	//this->GetPanels();

	LoadData();


}

void RLNanoLeaf::LoadHooks()
{
	//Teamcolors
	gameWrapper->HookEvent("Function Engine.Pawn.GetTeam", std::bind(&RLNanoLeaf::LoadTeams, this, std::placeholders::_1));
	//

	//Stats Feature - Goals and Demos events
	gameWrapper->HookEventWithCallerPost<ActorWrapper>("Function TAGame.GFxHUD_TA.HandleStatTickerMessage",
		[this](ActorWrapper caller, void* params, std::string eventname) {
			StatsHook(params);
		});
	//

	//Overtime
	gameWrapper->HookEvent("Function TAGame.GameEvent_Soccar_TA.OnOvertimeUpdated", std::bind(&RLNanoLeaf::OvertimeHook, this, std::placeholders::_1));
	//


	//Main Menu
	gameWrapper->HookEvent("Function TAGame.GFxData_MainMenu_TA.MainMenuAdded", std::bind(&RLNanoLeaf::MainMenuHook, this, std::placeholders::_1));
	//

	//Match Countdown
	gameWrapper->HookEvent("Function ProjectX.OnlineGameJoinGame_X.StartJoin", std::bind(&RLNanoLeaf::MatchCountdownHook, this, std::placeholders::_1));
	gameWrapper->HookEvent("Function ProjectX.OnlineGameMatchmakingBase_X.Joining.Cancel", std::bind(&RLNanoLeaf::MainMenuHook, this, std::placeholders::_1));
	gameWrapper->HookEvent("Function ProjectX.OnlineGameMatchmakingBase_X.Cancel", std::bind(&RLNanoLeaf::MainMenuHook, this, std::placeholders::_1));

	//

	//On Game Exit
	gameWrapper->HookEvent("Function ProjectX.GFxShell_X.ExitGame", std::bind(&RLNanoLeaf::ExitHook, this, std::placeholders::_1));
	//

	//Check if it is a replay
	gameWrapper->HookEvent("Function GameEvent_Soccar_TA.ReplayPlayback.BeginState", std::bind(&RLNanoLeaf::Replay, this, std::placeholders::_1));
	gameWrapper->HookEvent("Function GameEvent_Soccar_TA.ReplayPlayback.EndState", std::bind(&RLNanoLeaf::NotReplay, this, std::placeholders::_1));
	//

	RLNanoLeaf::GetEffects([this](const std::string& result) {});
}


void RLNanoLeaf::LoadTeams(std::string name)
{
	//Check if plugin is enabled
	CVarWrapper enableCvar = cvarManager->getCvar("cl_rln_enabled");
	bool enabled = enableCvar.getBoolValue();
	if (!enabled) { LOG("RLNanoLeaf is not enabled"); return; }

	//See if team colors are enabled
	CVarWrapper teamsEnabledCvar = cvarManager->getCvar("cl_rln_teams_enabled");
	bool teamsEnabled = teamsEnabledCvar.getBoolValue();
	if (!teamsEnabled) { LOG("Team color Lights are not enabled"); return; }

	//Check if it is a replay (this is may be temporary to minimize log flooding on Home Assistant)
	CVarWrapper replayCvar = cvarManager->getCvar("cl_rln_isReplay");
	bool isReplay = replayCvar.getBoolValue();
	if (isReplay == true) { Log("It's a replay"); return; }



	//Get player team and primary color
	if (gameWrapper->IsInFreeplay()) {

		CVarWrapper freeplayEnabledCvar = cvarManager->getCvar("cl_rln_freeplay_enabled");
		bool freeplayEnabled = freeplayEnabledCvar.getBoolValue();

		if (freeplayEnabled == true) {
			LOG("Player in freeplay, using freeplay hook");
			FreeplayHook();
			return;
		}

	}

	if (!gameWrapper->IsInFreeplay()) {


		CVarWrapper replayCvar = cvarManager->getCvar("cl_rln_isReplay");
		bool isReplay = replayCvar.getBoolValue();
		if (isReplay == true) { Log("It's a replay"); return; }

		cvarManager->log("Player in game, using team colors");

		ServerWrapper server = gameWrapper->GetCurrentGameState();
		if (!server) { LOG("Server nullcheck failed");   return; }

		auto primary = server.GetLocalPrimaryPlayer();
		if (!primary) { LOG("Server primary nullcheck failed");   return; }

		auto teams = server.GetTeams();
		if (teams.Count() < 2) { return; }

		auto pris = primary.GetPRI();
		if (!pris) { LOG("Server pri's nullcheck failed");   return; }

		//Get player's team number - 0 = Blue, 1 = Orange
		int teamnum = pris.GetTeamNum();
		if (teamnum > 1) { LOG("teamNum check failed");   return; }

		TeamWrapper myTeam = teams.Get(teamnum);
		if (teamnum > 1) { LOG("teamNum check failed");   return; }

		int otherteamnum = 0;

		if (teamnum == 0) {

			otherteamnum = 1;

		}

		if (teamnum == 1) {

			otherteamnum = 0;

		}

		TeamWrapper otherTeam = teams.Get(otherteamnum);


		//set other team's number based on player's current team number
		if (teamnum == 0) {

			otherteamnum = 1;
			TeamWrapper otherTeam = teams.Get(otherteamnum);
		}

		if (teamnum == 1) {

			otherteamnum = 0;
			TeamWrapper otherTeam = teams.Get(otherteamnum);
		}

		//Get player's team's linear color
		LinearColor primaryColor = myTeam.GetPrimaryColor();
		LinearColor otherPrimaryColor = otherTeam.GetPrimaryColor();


		//Begin linear to RGB convertion
		float myTeamPrimaryRed = primaryColor.R;
		float myTeamPrimaryGreen = primaryColor.G;
		float myTeamPrimaryBlue = primaryColor.B;


		float otherTeamPrimaryRed = otherPrimaryColor.R;
		float otherTeamPrimaryGreen = otherPrimaryColor.G;
		float otherTeamPrimaryBlue = otherPrimaryColor.B;

		CVarWrapper haMyTeamPrimaryRGBColorCvar = cvarManager->getCvar("cl_rln_myTeamPrimaryRGBColor");

		if (!haMyTeamPrimaryRGBColorCvar) { return; }

		CVarWrapper haOtherTeamPrimaryRGBColorCvar = cvarManager->getCvar("cl_rln_OtherTeamPrimaryRGBColor");

		if (!haOtherTeamPrimaryRGBColorCvar) { return; }

		//call to convert player's team's linear color to RGB
		std::string myTeamPrimaryRGB = ConvertLinearColor(myTeamPrimaryRed, myTeamPrimaryGreen, myTeamPrimaryBlue);
		haMyTeamPrimaryRGBColorCvar.setValue(myTeamPrimaryRGB);

		//call to convert other team's linear color to RGB
		std::string otherTeamPrimaryRGB = ConvertLinearColor(otherTeamPrimaryRed, otherTeamPrimaryGreen, otherTeamPrimaryBlue);
		haOtherTeamPrimaryRGBColorCvar.setValue(otherTeamPrimaryRGB);



		//get current team cvar
		CVarWrapper ha_playersTeam = cvarManager->getCvar("cl_rln_playersTeam");
		if (!ha_playersTeam) { return; }

		CVarWrapper ha_otherTeam = cvarManager->getCvar("cl_rln_otherTeam");
		if (!ha_otherTeam) { return; }


		//Send based on home or away team
		if (teamnum <= 1) {

			if (teamnum == 0) {

				std::string event = "home";
				ha_playersTeam.setValue(teamnum);
				ha_otherTeam.setValue(1);
				LOG("Using Home Team Colors");
				SendCommands(event, haMyTeamPrimaryRGBColorCvar.getColorValue());
			}

			if (teamnum == 1) {
				std::string event = "away";
				ha_playersTeam.setValue(teamnum);
				ha_otherTeam.setValue(0);
				LOG("Using Away Team Colors");
				SendCommands(event, haMyTeamPrimaryRGBColorCvar.getColorValue());
			}

		}
	}
}

std::string RLNanoLeaf::ConvertLinearColor(float red, float green, float blue)
{

	//convert linear color 0.1-1 to RGB 255
	//
	//convert primaryColor.R - (red)
	auto var_tempR = red * 255;
	auto var_R = (int)var_tempR;
	std::string R = std::to_string(var_R);

	//convert primaryColor.G - (green)
	auto var_tempG = green * 255;
	auto var_G = (int)var_tempG;
	std::string G = std::to_string(var_G);

	//convert primaryColor.B - (blue)
	auto var_tempB = blue * 255;
	auto var_B = (int)var_tempB;
	std::string B = std::to_string(var_B);


	//Formats to JSON
	std::string rgbColors = "("+ R + "," + G + "," + B + ",1)";
	//LOG("These the RGB colors{}", rgbColors);
	return rgbColors;

}


void RLNanoLeaf::StatsHook(void* params)
{

	//Check if plugin is enabled
	CVarWrapper enableCvar = cvarManager->getCvar("cl_rln_enabled");
	bool enabled = enableCvar.getBoolValue();

	if (!enabled) { LOG("RLNanoLeaf is not enabled"); return; }

	//See if demos are enabled
	CVarWrapper demosEnabledCvar = cvarManager->getCvar("cl_rln_demos_enabled");
	bool demosEnabled = demosEnabledCvar.getBoolValue();


	//See if it is a replay
	CVarWrapper replayCvar = cvarManager->getCvar("cl_rln_isReplay");
	bool isReplay = replayCvar.getBoolValue();
	if (isReplay == true) { Log("It's a replay"); return; }

	//Current team CVAR
	//get current team cvar
	CVarWrapper ha_playersTeam = cvarManager->getCvar("cl_rln_playersTeam");
	if (!ha_playersTeam) { return; }
	//
	auto haplayersTeam = cvarManager->getCvar("cl_rln_playersTeam");
	if (!haplayersTeam) { return; }
	int haplayersTeam2 = haplayersTeam.getIntValue();

	//get current otherteam cvar
	CVarWrapper ha_otherTeam = cvarManager->getCvar("cl_rln_otherTeam");
	if (!ha_otherTeam) { return; }
	//
	auto haotherTeam = cvarManager->getCvar("cl_rln_otherTeam");
	if (!haotherTeam) { return; }
	int haotherTeam2 = haotherTeam.getIntValue();


	CVarWrapper teamDemoColorEnabledCvar = cvarManager->getCvar("cl_rln_teamDemoColor_enabled");
	bool teamDemoColorEnabled = teamDemoColorEnabledCvar.getBoolValue();

	CVarWrapper teamGoalColorEnabledCvar = cvarManager->getCvar("cl_rln_teamGoalColor_enabled");
	bool teamGoalColorEnabled = teamGoalColorEnabledCvar.getBoolValue();


	//setup teamwrapper

	ServerWrapper server = gameWrapper->GetCurrentGameState();
	if (!server) { LOG("Server nullcheck failed");   return; }

	auto primary = server.GetLocalPrimaryPlayer();
	if (!primary) { LOG("Server primary nullcheck failed");   return; }

	auto teams = server.GetTeams();
	if (teams.Count() < 2) { return; }



	//Get a StatEvent's info
	struct StatTickerParams {
		uintptr_t Receiver;
		uintptr_t Victim;
		uintptr_t StatEvent;
	};
	StatTickerParams* pStruct = (StatTickerParams*)params;
	PriWrapper receiver = PriWrapper(pStruct->Receiver);
	PriWrapper victim = PriWrapper(pStruct->Victim);
	StatEventWrapper statEvent = StatEventWrapper(pStruct->StatEvent);

	//Get team colors

	CVarWrapper haMyTeamPrimaryRGBColorCvar = cvarManager->getCvar("cl_rln_myTeamPrimaryRGBColor");
	if (!haMyTeamPrimaryRGBColorCvar) { return; }
	LinearColor haMyTeamPrimaryRGBColor = haMyTeamPrimaryRGBColorCvar.getColorValue();

	CVarWrapper haOtherTeamPrimaryRGBColorCvar = cvarManager->getCvar("cl_rln_OtherTeamPrimaryRGBColor");
	if (!haOtherTeamPrimaryRGBColorCvar) { return; }
	LinearColor haOtherTeamPrimaryRGBColor = haOtherTeamPrimaryRGBColorCvar.getColorValue();

	//LOG("StatEventOccured");
	if (statEvent.GetEventName() == "Goal") {
		LOG("goalScored lights active");
		//See if Goal Scored hook is enabled
		CVarWrapper goalScoredEnabledCvar = cvarManager->getCvar("cl_rln_goalScored_enabled");
		bool goalScoredEnabled = goalScoredEnabledCvar.getBoolValue();
		if (!goalScoredEnabled) { LOG("goalScored Lights are not enabled"); return; }

		//See if it is a replay
		CVarWrapper replayCvar = cvarManager->getCvar("cl_rln_isReplay");
		bool isReplay = replayCvar.getBoolValue();
		if (isReplay == true) { Log("It's a replay"); return; }


		int tmpCounter = 0;
		int lastGoalScoredBy = receiver.GetTeamNum();

		CVarWrapper goalScoredColorVar = cvarManager->getCvar("cl_rln_goalScored_color");
		if (!goalScoredColorVar) { return; }
		LinearColor goalScoredColor = goalScoredColorVar.getColorValue();




		LOG("goalScored lights active");

		std::string event = "goal";



		//Compare team number to players team to decide who scored

		if (teamGoalColorEnabled)
		{

			if (lastGoalScoredBy <= 1) {

				if (lastGoalScoredBy == haplayersTeam2) {
					std::string event = "temaScored";
					LOG("Your team scored");

					goalScoredColor.R = (haMyTeamPrimaryRGBColor.R + ((255 - haMyTeamPrimaryRGBColor.R) / 2));
					goalScoredColor.G = (haMyTeamPrimaryRGBColor.G + ((255 - haMyTeamPrimaryRGBColor.G) / 2));
					goalScoredColor.B = (haMyTeamPrimaryRGBColor.B + ((255 - haMyTeamPrimaryRGBColor.B) / 2));

					SendCommands(event, goalScoredColor);



				}

				if (lastGoalScoredBy != haplayersTeam2) {
					std::string event = "otherTeamScored";
					LOG("Other team scored");


					goalScoredColor.R = (haOtherTeamPrimaryRGBColor.R + ((255 - haOtherTeamPrimaryRGBColor.R) / 2));
					goalScoredColor.G = (haOtherTeamPrimaryRGBColor.G + ((255 - haOtherTeamPrimaryRGBColor.G) / 2));
					goalScoredColor.B = (haOtherTeamPrimaryRGBColor.B + ((255 - haOtherTeamPrimaryRGBColor.B) / 2));


					SendCommands(event, goalScoredColor);
				}

			}
		}
		else
		{
			SendCommands(event, goalScoredColor);
		}


		//LOG("Other Team's Score: {}", teams.Get(haotherTeam2).GetScore());
		//LOG("Your Team's Score: {}", teams.Get(haplayersTeam2).GetScore());
		//
		//LOG("Using Goals Hook", lastGoalScoredBy);
	}
	//LOG("{}", statEvent.GetEventName());

	if (statEvent.GetEventName() == "Demolish") {
		LOG("Demo Occured");
		CVarWrapper demosEnabledCvar = cvarManager->getCvar("cl_rln_demos_enabled");
		bool demosEnabled = demosEnabledCvar.getBoolValue();
		if (!demosEnabled) { LOG("Demos Lights are not enabled"); return; }

		if (!receiver) { LOG("Null reciever PRI"); return; }
		if (!victim) { LOG("Null victim PRI"); return; }

		// Find the primary player's PRI
		PlayerControllerWrapper playerController = gameWrapper->GetPlayerController();
		if (!playerController) { LOG("Null controller"); return; }
		PriWrapper playerPRI = playerController.GetPRI();
		if (!playerPRI) { LOG("Null player PRI"); return; }

		int receiversTeam = receiver.GetTeamNum();
		int victimsTeam = victim.GetTeamNum();
		int currentTeam = haplayersTeam2;

		//LOG("test: {}", testInt);
		//LOG("receiver: {}", receiversTeam);
		//LOG("victim: {}", victimsTeam);
		//LOG("Current team number: {}", currentTeam);

		CVarWrapper demoColorVar = cvarManager->getCvar("cl_rln_demo_color");
		if (!demoColorVar) { return; }
		LinearColor demoColor = demoColorVar.getColorValue();
		std::string event = "demo";

		if (teamDemoColorEnabled) {
			if (receiversTeam == haplayersTeam2) {
				std::string event = "teamDemoed";
				LOG("Using Demos Hook");

				demoColor.R = (haMyTeamPrimaryRGBColor.R + ((255 - haMyTeamPrimaryRGBColor.R) / 2));
				demoColor.G = (haMyTeamPrimaryRGBColor.G + ((255 - haMyTeamPrimaryRGBColor.G) / 2));
				demoColor.B = (haMyTeamPrimaryRGBColor.B + ((255 - haMyTeamPrimaryRGBColor.B) / 2));

				SendCommands(event, demoColor);
			}

			if (receiversTeam != haplayersTeam2) {
				std::string event = "gotDemoed";
				LOG("Using Demos Hook");

				demoColor.R = (haMyTeamPrimaryRGBColor.R + ((255 - haMyTeamPrimaryRGBColor.R) / 2));
				demoColor.G = (haMyTeamPrimaryRGBColor.G + ((255 - haMyTeamPrimaryRGBColor.G) / 2));
				demoColor.B = (haMyTeamPrimaryRGBColor.B + ((255 - haMyTeamPrimaryRGBColor.B) / 2));

				SendCommands(event, demoColor);
			}
		}
		else {

			SendCommands(event, demoColor);
		}

	}

}

void RLNanoLeaf::FreeplayHook()
{
	//Check if plugin is enabled
	CVarWrapper enableCvar = cvarManager->getCvar("cl_rln_enabled");
	bool enabled = enableCvar.getBoolValue();
	if (!enabled) { LOG("RLNanoLeaf is not enabled"); return; }

	//See if FreePlay Hooks are enabled
	CVarWrapper freeplay_enabledCvar = cvarManager->getCvar("cl_rln_freeplay_enabled");
	bool freeplay_enabled = freeplay_enabledCvar.getBoolValue();
	if (!freeplay_enabled) { LOG("Freeplay Lights are not enabled"); return; }

	CVarWrapper freeplayColorVar = cvarManager->getCvar("cl_rln_freeplay_color");
	if (!freeplayColorVar) { return; }
	LinearColor freeplayColor = freeplayColorVar.getColorValue();

	CVarWrapper effectsEnableCvar = cvarManager->getCvar("cl_rln_effects_enabled");
	if (!effectsEnableCvar) { return; }
	bool effectsEnabbled = effectsEnableCvar.getBoolValue();

	//May be redundant, but good to check
	if (gameWrapper->IsInFreeplay()) {
		if (effectsEnabbled) {
			//send effect
			SendCommands("freeplay");
		}
		else
		{
			//Send freeplay color
			std::string event = "freeplay";
			LOG("Sending freeplay commands");
			SendCommands(event, freeplayColor);
		}


	}
}

void RLNanoLeaf::MainMenuHook(std::string name)
{

	LOG("MainMenu Hook");

	//Check if plugin is enabled
	CVarWrapper enableCvar = cvarManager->getCvar("cl_rln_enabled");
	bool enabled = enableCvar.getBoolValue();

	if (!enabled) { LOG("RLNanoLeaf is not enabled"); return; }

	//See if main menu hook is enabled
	CVarWrapper mainmenuEnabledCvar = cvarManager->getCvar("cl_rln_mainmenu_enabled");
	bool mainmenuEnabled = mainmenuEnabledCvar.getBoolValue();
	if (!mainmenuEnabled) { LOG("Main Menu Lights are not enabled"); return; }

	//Check if player is in Freeplay (This is checked because the MainMenuHook function is called from the cancellation of matchmaking)
	if (gameWrapper->IsInFreeplay()) {

		CVarWrapper freeplayEnabledCvar = cvarManager->getCvar("cl_rln_freeplay_enabled");
		bool freeplayEnabled = freeplayEnabledCvar.getBoolValue();

		if (freeplayEnabled == true) {
			LOG("Player in freeplay, using freeplay hook");
			FreeplayHook();
			return;
		}

	}

	//Check if player is in a game (This is checked because the MainMenuHook function is called from the cancellation of matchmaking)
	if ((gameWrapper->IsInGame()) || (gameWrapper->IsInOnlineGame())) {

		CVarWrapper teamsEnabledCvar = cvarManager->getCvar("cl_rln_teams_enabled");
		bool teamsEnabled = teamsEnabledCvar.getBoolValue();

		CVarWrapper effectsEnableCvar = cvarManager->getCvar("cl_rln_effects_enabled");
		if (!effectsEnableCvar) { return; }
		bool effectsEnabbled = effectsEnableCvar.getBoolValue();

		if (effectsEnabbled) {
			//send effect
			SendCommands("mainmenu");
		}
		else
		{

			if (teamsEnabled == true) {
				LOG("Player in a game, using teamshook");
				LoadTeams("loadteams");
				return;
			}
		}
	}

	CVarWrapper mainmenuColorVar = cvarManager->getCvar("cl_rln_mainmenu_color");
	if (!mainmenuColorVar) { return; }
	LinearColor mainmenuColor = mainmenuColorVar.getColorValue();

	//Send mainmenu color
	std::string event = "mainmenu";
	LOG("Using Main Menu Hook");
	SendCommands(event, mainmenuColor);

}

void RLNanoLeaf::MatchCountdownHook(std::string name)
{
	//Check if plugin is enabled
	CVarWrapper enableCvar = cvarManager->getCvar("cl_rln_enabled");
	bool enabled = enableCvar.getBoolValue();
	if (!enabled) { LOG("RLNanoLeaf is not enabled"); return; }

	//See if MatchCountdown Hooks are enabled
	CVarWrapper matchCountdown_enabledCvar = cvarManager->getCvar("cl_rln_matchCountdown_enabled");
	bool matchCountdown_enabled = matchCountdown_enabledCvar.getBoolValue();
	if (!matchCountdown_enabled) { LOG("Match Countdown Lights are not enabled"); return; }

	CVarWrapper matchCountdownColorVar = cvarManager->getCvar("cl_rln_matchCountdown_color");
	if (!matchCountdownColorVar) { return; }
	LinearColor matchCountdownColor = matchCountdownColorVar.getColorValue();

	//Send matchcountdown color
	std::string event = "matchcountdown";
	LOG("Using Match Countdown Hook");
	SendCommands(event, matchCountdownColor);

}

void RLNanoLeaf::OvertimeHook(std::string name)
{
	//Check if plugin is enabled
	CVarWrapper enableCvar = cvarManager->getCvar("cl_rln_enabled");
	bool enabled = enableCvar.getBoolValue();

	if (!enabled) { LOG("RLNanoLeaf is not enabled"); return; }


	//See if overtime hook is enabled
	CVarWrapper overtimeEnabledCvar = cvarManager->getCvar("cl_rln_overtime_enabled");
	bool overtimeEnabled = overtimeEnabledCvar.getBoolValue();
	if (!overtimeEnabled) { LOG("Overtime Lights are not enabled"); return; }

	CVarWrapper overtimeColorVar = cvarManager->getCvar("cl_rln_overtime_color");
	if (!overtimeColorVar) { return; }
	LinearColor overtimeColor = overtimeColorVar.getColorValue();

	CVarWrapper effectsEnableCvar = cvarManager->getCvar("cl_rln_effects_enabled");
	if (!effectsEnableCvar) { return; }
	bool effectsEnabbled = effectsEnableCvar.getBoolValue();

	//Send overtime color
	if (effectsEnabbled) {
		//send effect
		SendCommands("overtime");
	}
	else
	{
		std::string event = "overtime";
		LOG("Using Overtime Hook");
		SendCommands(event, overtimeColor);
	}


}

void RLNanoLeaf::ExitHook(std::string name)
{

	//Check if plugin is enabled
	CVarWrapper enableCvar = cvarManager->getCvar("cl_rln_enabled");
	bool enabled = enableCvar.getBoolValue();

	if (!enabled) { LOG("RLNanoLeaf is not enabled"); return; }

	//See if exit hook is enabled
	CVarWrapper exitEnabledCvar = cvarManager->getCvar("cl_rln_exit_enabled");
	bool exitEnabled = exitEnabledCvar.getBoolValue();
	if (!exitEnabled) { LOG("Exit Lights are not enabled"); return; }

	CVarWrapper exitColorVar = cvarManager->getCvar("cl_rln_exit_color");
	if (!exitColorVar) { return; }
	LinearColor exitColor = exitColorVar.getColorValue();

	std::string event = "exit";
	LOG("Using Exit Hook");
	SendCommands(event, exitColor);

}


void RLNanoLeaf::Replay(std::string name)
{
	//LOG("Replay start");
	CVarWrapper replayCvar = cvarManager->getCvar("cl_rln_isReplay");
	bool isReplay = replayCvar.getBoolValue();
	isReplay = true;
	//LOG("{}", isReplay);
	replayCvar.setValue(isReplay);


}

void RLNanoLeaf::NotReplay(std::string name)
{
	//LOG("Replay ended");
	CVarWrapper replayCvar = cvarManager->getCvar("cl_rln_isReplay");
	bool isReplay = replayCvar.getBoolValue();
	isReplay = false;
	//LOG("{}", isReplay);
	replayCvar.setValue(isReplay);

}

int RLNanoLeaf::GetScore(int teamNum)
{


	//setup teamwrapper
	//LOG("Getting score");
	ServerWrapper server = gameWrapper->GetCurrentGameState();
	if (!server) { LOG("Server nullcheck failed");   return 99; }

	auto primary = server.GetLocalPrimaryPlayer();
	if (!primary) { LOG("Server primary nullcheck failed");   return 99; }
	//LOG("TEAM NUM {}", teamNum);
	//LOG("gettings teams");
	auto teams = server.GetTeams();
	if (teams.Count() < 2) { return 99; }
	if (teams.Count() < 2) { return 99; }
	//LOG("Getting score2");
	int score = teams.Get(teamNum).GetScore();
	//LOG("Returning score");
	return score;

}

std::vector<int> RLNanoLeaf::PanelIDsToVector(std::string panelIdsStr) {
	////////   This is not currently used ///////////
	// Vector to store the parsed integers
	std::vector<int> panelIDs;

	// Create a stringstream from the input string
	std::istringstream iss(panelIdsStr);

	// Extract integers from the stream and push them into the vector
	int id;
	while (iss >> id) {
		panelIDs.push_back(id);
	}

	// Return the vector
	return panelIDs;

}

void RLNanoLeaf::Log(std::string msg)
{
	//Send logs to BakkesMod console
	cvarManager->log(msg);
}



void RLNanoLeaf::LoadData()
{
	// Upgrade old file path
	if (std::filesystem::exists(this->mainFile)) {
		Log("Upgrading old file path");
		std::filesystem::create_directories(this->dataDir);
		std::filesystem::rename(this->mainFile, this->mainPath);
		std::filesystem::rename(this->bakFile, this->bakPath);
	}

	std::ifstream in(this->mainPath);
	if (in.fail()) {
		LOG("Failed to open file");
		LOG(strerror(errno));
		this->data["nanloeaf"] = json::object();
		WriteData();
		in.open(this->mainPath);
	}

	try {
		in >> this->data;
	}
	catch (const nlohmann::detail::exception& e) {
		in.close();
		LOG("Failed to parse json");
		LOG(e.what());
	}

	if (!this->data.contains("nanoleaf")) {
		Log("Data doesn't contain nanoleaf");
		this->data["nanoleaf"] = json::object();
	}

	Log("Successfully loaded existing data");

	in.close();

	WriteData();

}


void RLNanoLeaf::WriteData()
{
	LOG("WriteData");
	std::filesystem::create_directories(this->dataDir);

	std::ofstream out(this->tmpPath);
	try {
		out << this->data.dump(4, ' ', false, json::error_handler_t::replace) << std::endl;
		out.close();
		std::error_code err;
		std::filesystem::remove(this->bakPath, err);
		if (std::filesystem::exists(this->mainPath)) {
			std::filesystem::rename(this->mainPath, this->bakPath, err);
			if (err.value() != 0) {
				LOG("Could not backup NanoLeaf Data");
				LOG(err.message());
				return;
			}
		}
		std::filesystem::rename(this->tmpPath, this->mainPath, err);
		if (err.value() != 0) {
			LOG("Could not move NanoLeaf temp file to main");
			LOG(err.message());
			std::filesystem::rename(this->bakPath, this->mainPath, err);
			return;
		}
	}
	catch (const nlohmann::detail::exception& e) {
		LOG("failed to serialize json");
		LOG(e.what());
	}
	catch (...) {
		LOG("failed to serialize json (unknown)");
	}
}

void RLNanoLeaf::DisplayEffectPopup(const char* label, CVarWrapper& effectVar, std::vector<std::string>& effects) {
	// Static variable to keep track of selected effect index for this specific popup
	static int selectedEffect = -1;

	// Button to open the popup
	if (ImGui::Button(label)) {
		ImGui::OpenPopup(label);
	}

	ImGui::SameLine();
	std::string currentEffect = effectVar.getStringValue();
	ImGui::TextUnformatted(currentEffect.empty() ? "<None>" : currentEffect.c_str());
	// Create the simple popup for selecting effects
	if (ImGui::BeginPopup(label)) {
		for (int i = 0; i < effects.size(); i++) {
			if (ImGui::Selectable(effects[i].c_str())) {
				selectedEffect = i;  // Update the selected effect

				// Set the selected effect using the effectVar
				effectVar.setValue(effects[selectedEffect]);

				LOG("Selected effect for {}: {}", label, effects[selectedEffect]);
			}
		}
		ImGui::EndPopup();
	}
	cvarManager->executeCommand("writeconfig", false);
}
