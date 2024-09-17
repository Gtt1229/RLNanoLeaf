#pragma once

#include "GuiBase.h"
#include "bakkesmod/plugin/bakkesmodplugin.h"
#include "bakkesmod/plugin/pluginwindow.h"
#include "bakkesmod/plugin/PluginSettingsWindow.h"

#include "version.h"
#include "pch.h"
constexpr auto plugin_version = stringify(VERSION_MAJOR) "." stringify(VERSION_MINOR) "." stringify(VERSION_PATCH) "." stringify(VERSION_BUILD);


constexpr auto CVAR_DEMO_COLOR = "cl_rln_demo_color";


class RLNanoLeaf: public BakkesMod::Plugin::BakkesModPlugin
	,public SettingsWindowBase // Uncomment if you wanna render your own tab in the settings menu
	,public PluginWindowBase // Uncomment if you want to render your own plugin window
{

	//std::shared_ptr<bool> enabled;

	//Boilerplate
	void onLoad() override;

	//void onUnload() override; // Uncomment and implement if you need a unload method

		//Custom methods
	void LoadHooks();
	void LoadTeams(std::string name);
	std::string ConvertLinearColor(float red, float green, float blue);
	void StatsHook(void* params);
	//void ChatHook(void* params);
	void FreeplayHook();
	void MainMenuHook(std::string name);
	void MatchCountdownHook(std::string name);
	void OvertimeHook(std::string name);
	//void GoalScoredHook(std::string name);
	void ExitHook(std::string name);
	void Replay(std::string name);
	void NotReplay(std::string name);
	void SendCommands(std::string event, LinearColor color);
	void GetAuthToken();
	void GetPanels();
	void SetAuthToken(std::string token);
	int GetScore(int teamNum);
	std::vector<int> PanelIDsToVector(std::string panelIdsStr);



public:
	void RenderSettings() override; // Uncomment if you wanna render your own tab in the settings menu
	std::string GetPluginName() override;
	bool isWindowOpen_ = false;
	bool isMinimized_ = false;
	std::string menuTitle_ = "RocketLeagueAssistant";
	virtual void Render() override;
	virtual std::string GetMenuName() override;
	virtual std::string GetMenuTitle() override;
	virtual void SetImGuiContext(uintptr_t ctx) override;
	virtual bool ShouldBlockInput() override;
	virtual bool IsActiveOverlay() override;
	virtual void OnOpen() override;
	virtual void OnClose() override;
	
	//void RenderWindow() override; // Uncomment if you want to render your own plugin window
	struct HSV {
		int h;  // angle in degrees
		int s;  // fraction between 0 and 1
		int v;  // fraction between 0 and 1
	};

	struct RGB {
		double r;  // fraction between 0 and 1
		double g;  // fraction between 0 and 1
		double b;  // fraction between 0 and 1
	};
	HSV rgb2hsv(const RGB& in);

private:

	//Logging method
	void Log(std::string msg);

};
