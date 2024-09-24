#pragma once

#include "GuiBase.h"
#include "bakkesmod/plugin/bakkesmodplugin.h"
#include "bakkesmod/plugin/pluginwindow.h"
#include "bakkesmod/plugin/PluginSettingsWindow.h"
#include <nlohmann/json.hpp>
#include "version.h"
#include "pch.h"

#include <vector>
#include <map>
#include <set>
#include <filesystem>
#include <optional>

constexpr auto plugin_version = stringify(VERSION_MAJOR) "." stringify(VERSION_MINOR) "." stringify(VERSION_PATCH) "." stringify(VERSION_BUILD);

using json = nlohmann::json;



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
	void SendCommands(std::string effect);
	void GetAuthToken();
	void GetPanels();
	void SetAuthToken(std::string token);
	int GetScore(int teamNum);
	std::vector<int> PanelIDsToVector(std::string panelIdsStr);

	using GetEffectsCallback = std::function<void(const std::string&)>;
	void GetEffects(GetEffectsCallback callback);


public:
	void RenderSettings() override; // Uncomment if you wanna render your own tab in the settings menu
	std::string GetPluginName() override;
	void DisplayEffectPopup(const char* label, CVarWrapper& effectVar, std::vector<std::string>& effects);
	bool isWindowOpen_ = false;
	bool isMinimized_ = false;
	std::string menuTitle_ = "RLNanoLeaf";
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
	json data;
	//Logging method
	void Log(std::string msg);

	inline static auto mainFile = "nanoleafData.json";
	inline static auto tmpFile = "nanoleafData.json.tmp";
	inline static auto bakFile = "nanoleafData.json.bak";

	inline static std::filesystem::path dataDir;
	inline static std::filesystem::path mainPath;
	inline static std::filesystem::path tmpPath;
	inline static std::filesystem::path bakPath;
	void LoadData();
	void WriteData();
};
