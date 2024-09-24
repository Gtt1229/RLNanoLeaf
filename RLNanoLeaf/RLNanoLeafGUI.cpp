#include "pch.h"
#include "RLNanoLeaf.h"
#include "IMGUI/imgui_internal.h"
#include "IMGUI/imgui_searchablecombo.h"
#include "IMGUI/imgui_stdlib.h"
#include <filesystem>
#include <fstream>  


// Plugin Settings Window code here
std::string RLNanoLeaf::GetPluginName() {
	return "RLNanoLeaf";
}

void RLNanoLeaf::SetImGuiContext(uintptr_t ctx) {
	ImGui::SetCurrentContext(reinterpret_cast<ImGuiContext*>(ctx));
}

// Render the plugin settings here
// This will show up in bakkesmod when the plugin is loaded at
//  f2 -> plugins -> RLNanoLeaf
void RLNanoLeaf::RenderSettings() {

	//Get related Cvars
	CVarWrapper enableCvar = cvarManager->getCvar("cl_rln_enabled");

	CVarWrapper teamsEnableCvar = cvarManager->getCvar("cl_rln_teams_enabled");

	CVarWrapper goalScoredEnableCvar = cvarManager->getCvar("cl_rln_goalScored_enabled");

	CVarWrapper mainmenuEnableCvar = cvarManager->getCvar("cl_rln_mainmenu_enabled");

	CVarWrapper matchCountdownEnableCvar = cvarManager->getCvar("cl_rln_matchCountdown_enabled");

	CVarWrapper demosEnableCvar = cvarManager->getCvar("cl_rln_demos_enabled");

	CVarWrapper overtimeEnableCvar = cvarManager->getCvar("cl_rln_overtime_enabled");

	CVarWrapper freeplayEnableCvar = cvarManager->getCvar("cl_rln_freeplay_enabled");

	CVarWrapper exitEnableCvar = cvarManager->getCvar("cl_rln_exit_enabled");
	CVarWrapper exitOffCvar = cvarManager->getCvar("cl_rln_exit_off");

	//CVarWrapper loggingEnableCvar = cvarManager->getCvar("cl_rln_logging");

	CVarWrapper teamDemoColorEnableCvar = cvarManager->getCvar("cl_rln_teamDemoColor_enabled");

	CVarWrapper teamGoalColorEnableCvar = cvarManager->getCvar("cl_rln_teamGoalColor_enabled");

	CVarWrapper nanoLeafIPCvar = cvarManager->getCvar("cl_rln_nanoLeafIP");

	CVarWrapper nanoLeafTokenCvar = cvarManager->getCvar("cl_rln_nanoLeaftoken");

	CVarWrapper hideURLCvar = cvarManager->getCvar("hideURL");

	CVarWrapper panelIDsCvar = cvarManager->getCvar("cl_rln_panelIDs");

	CVarWrapper effectsEnableCvar = cvarManager->getCvar("cl_rln_effects_enabled");

	if (!enableCvar) { return; }
	if (!teamsEnableCvar) { return; }
	if (!goalScoredEnableCvar) { return; }
	if (!overtimeEnableCvar) { return; }
	if (!demosEnableCvar) { return; }
	if (!freeplayEnableCvar) { return; }
	if (!exitEnableCvar) { return; }
	if (!exitOffCvar) { return; }
	//if (!loggingEnableCvar) { return; }
	if (!teamDemoColorEnableCvar) { return; }
	if (!teamGoalColorEnableCvar) { return; }
	if (!mainmenuEnableCvar) { return; }
	if (!matchCountdownEnableCvar) { return; }
	if (!panelIDsCvar) { return; }
	if (!effectsEnableCvar) { return; }

	//Colors
	CVarWrapper demoColorVar = cvarManager->getCvar("cl_rln_demo_color");
	if (!demoColorVar) { return; }
	CVarWrapper goalScoredColorVar = cvarManager->getCvar("cl_rln_goalScored_color");
	if (!goalScoredColorVar) { return; }
	CVarWrapper freeplayColorVar = cvarManager->getCvar("cl_rln_freeplay_color");
	if (!freeplayColorVar) { return; }
	CVarWrapper mainmenuColorVar = cvarManager->getCvar("cl_rln_mainmenu_color");
	if (!mainmenuColorVar) { return; }
	CVarWrapper overtimeColorVar = cvarManager->getCvar("cl_rln_overtime_color");
	if (!overtimeColorVar) { return; }
	CVarWrapper matchCountdownColorVar = cvarManager->getCvar("cl_rln_matchCountdown_color");
	if (!matchCountdownColorVar) { return; }
	CVarWrapper exitColorVar = cvarManager->getCvar("cl_rln_exit_color");
	if (!exitColorVar) { return; }
	CVarWrapper brightnessVar = cvarManager->getCvar("cl_rln_brightness");
	if (!brightnessVar) { return; }
	int brightness = brightnessVar.getIntValue();
	//Effects

	CVarWrapper freeplayEffectVar = cvarManager->getCvar("cl_rln_freeplay_effect");
	if (!freeplayEffectVar) { return; }
	CVarWrapper overtimeEffectVar = cvarManager->getCvar("cl_rln_overtime_effect");
	if (!overtimeEffectVar) { return; }
	CVarWrapper mainmenuEffectVar = cvarManager->getCvar("cl_rln_mainmenu_effect");
	if (!mainmenuEffectVar) { return; }
	CVarWrapper exitEffectVar = cvarManager->getCvar("cl_rln_exit_effect");
	if (!exitEffectVar) { return; }



	bool enabled = enableCvar.getBoolValue();
	bool hideURL = hideURLCvar.getBoolValue();
	bool exitOff = exitOffCvar.getBoolValue();
	bool exitEffect = exitEffectVar.getBoolValue();
	bool effectsEnabled = effectsEnableCvar.getBoolValue();
	//Enable plugin checkbox

	if (ImGui::Checkbox("Enable plugin", &enabled)) {
		enableCvar.setValue(enabled);
	}
	if (ImGui::IsItemHovered()) {
		ImGui::SetTooltip("Toggle Plugin");
	}

	
	//char const* currentUrl = reqhomeUrlex.data();

	if (enabled == true) {


		if (!nanoLeafIPCvar) { return; }
		std::string nanoLeafIPex = nanoLeafIPCvar.getStringValue();
		ImGui::PushItemWidth(33.0f * ImGui::GetFontSize());

		if (hideURL == false) {
			if (ImGui::InputText("NanoLeaf IP", &nanoLeafIPex)) {

				nanoLeafIPCvar.setValue(nanoLeafIPex);

			}
		}



		ImGui::Text("To receive your NanoLeaf Authentication Token:");

		ImGui::Text("1. Hold down power button for 5-7 seconds\n       Controller lights should flash");


		ImGui::Text("2."); ImGui::SameLine();

		if (ImGui::Button("Generate NanoLeaf Token")) {

			std::string nanoLeafIPex = nanoLeafIPCvar.getStringValue();
			RLNanoLeaf::GetAuthToken();

		}



		if (!nanoLeafTokenCvar) { return; }
		std::string nanoLeafTokenex = nanoLeafTokenCvar.getStringValue();
		ImGui::PushItemWidth(33.0f * ImGui::GetFontSize());
		if (hideURL == false) {
			if (ImGui::InputText("NanoLeaf Token", &nanoLeafTokenex)) {

				nanoLeafTokenCvar.setValue(nanoLeafTokenex);

			}
		}
		if (ImGui::Button("Get Panels (This is not used at this time)")) {

			RLNanoLeaf::GetPanels();

		}


		std::string panelIDsEx = panelIDsCvar.getStringValue();
		ImGui::Text("Curent Panels:"); ImGui::SameLine(); ImGui::InputText("", &panelIDsEx);

		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
		//Color Settings

		LinearColor demoColor = demoColorVar.getColorValue() / 255;
		LinearColor goalScoredColor = goalScoredColorVar.getColorValue() / 255;
		LinearColor freeplayColor = freeplayColorVar.getColorValue() / 255;
		LinearColor mainmenuColor = mainmenuColorVar.getColorValue() / 255;
		LinearColor overtimeColor = overtimeColorVar.getColorValue() / 255;
		LinearColor matchCountdownColor = matchCountdownColorVar.getColorValue() / 255;
		LinearColor exitColor = exitColorVar.getColorValue() / 255;


		bool teamDemoColorenabled = teamDemoColorEnableCvar.getBoolValue();
		bool teamGoalColorenabled = teamGoalColorEnableCvar.getBoolValue();


		static int brightnessSlider = brightnessVar.getIntValue();
		if (ImGui::SliderInt("slider int", &brightnessSlider, 0, 100)) {

			brightnessVar.setValue(brightnessSlider);

		}


		if (ImGui::Checkbox("Enable Demos Based on Teams' Color", &teamDemoColorenabled)) {
			teamDemoColorEnableCvar.setValue(teamDemoColorenabled);
		}
		if (ImGui::IsItemHovered()) {
			ImGui::SetTooltip("Toggle Effect");
		}

		if (!teamDemoColorenabled) {

			ImGui::Text("Demos Color:");
			ImGui::SameLine(200); if (ImGui::ColorEdit4("Demos Color", &demoColor.R, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel))
			{
				demoColorVar.setValue(demoColor * 255);
				SendCommands("teamDemoed", demoColor);
			}

		}

		if (ImGui::Checkbox("Enable Goals Based on Teams' Color", &teamGoalColorenabled)) {
			teamGoalColorEnableCvar.setValue(teamGoalColorenabled);
		}
		if (ImGui::IsItemHovered()) {
			ImGui::SetTooltip("Toggle Effect");
		}


		if (!teamGoalColorenabled) {

			ImGui::Text("Goals Scored Color:");
			ImGui::SameLine(150); if (ImGui::ColorEdit4("Goals Color", &goalScoredColor.R, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel))
			{
				goalScoredColorVar.setValue(goalScoredColor * 255);
				SendCommands("goalscored", goalScoredColor);
			}

		}
		if (!effectsEnabled) {
			ImGui::Text("Freeplay Color:");
			ImGui::SameLine(150); if (ImGui::ColorEdit4("Freeplay Color", &freeplayColor.R, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel))
			{
				freeplayColorVar.setValue(freeplayColor * 255);
				SendCommands("freeplay", freeplayColor);
			}

			ImGui::Text("Main Menu Color:");
			ImGui::SameLine(150); if (ImGui::ColorEdit4("Main Menu Color", &mainmenuColor.R, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel))
			{
				mainmenuColorVar.setValue(mainmenuColor * 255);
				SendCommands("mainmenu", mainmenuColor);
			}
		}
		if (!effectsEnabled) {
			ImGui::Text("Overtime Color:");
			ImGui::SameLine(150); if (ImGui::ColorEdit4("Overtime Color", &overtimeColor.R, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel))
			{
				overtimeColorVar.setValue(overtimeColor * 255);
				SendCommands("overtime", overtimeColor);
			}
		}
		ImGui::Text("Match Countdown Color:");
		ImGui::SameLine(150); if (ImGui::ColorEdit4("Match Countdown Color", &matchCountdownColor.R, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel))
		{
			matchCountdownColorVar.setValue(matchCountdownColor * 255);
			SendCommands("matchcountdown", matchCountdownColor);
		}

		if (!effectsEnabled) {
			if (ImGui::Checkbox("Turn off NanoLeaf on Exit", &exitOff)) {
				exitOffCvar.setValue(exitOff);
			}

			if (!exitOff)
			{
				ImGui::Text("Exit Color:");
				ImGui::SameLine(150); if (ImGui::ColorEdit4("Exit Color", &exitColor.R, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel))
				{
					exitColorVar.setValue(exitColor * 255);
					SendCommands("exit", exitColor);
				}
			}
		}

		if (ImGui::Button("Test Lights Using FreePlay Color")) {

			RLNanoLeaf::SendCommands("test", freeplayColorVar.getColorValue());

		}
		//static ImVec4 color_hsv(0.23f, 1.0f, 1.0f, 1.0f);
		
		//ImGui::Text("Color widget with InputHSV:");
		//ImGui::ColorEdit4("HSV shown as RGB##1", (float*)&color_hsv, ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_InputHSV | ImGuiColorEditFlags_Float);
		//ImGui::ColorEdit4("HSV shown as HSV##1", (float*)&color_hsv, ImGuiColorEditFlags_DisplayHSV | ImGuiColorEditFlags_InputHSV | ImGuiColorEditFlags_Float);
		//ImGui::DragFloat4("Raw HSV values", (float*)&color_hsv, 0.01f, 0.0f, 1.0f);

		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);


		if (ImGui::Checkbox("Use NanoLeaf Effects", &effectsEnabled)) {
			effectsEnableCvar.setValue(effectsEnabled);
		}
		if (ImGui::IsItemHovered()) {
			ImGui::SetTooltip("Toggle the use of NanoLeaf Effects");
		}
		if (effectsEnabled) {
			if (ImGui::TreeNode("Effects based colors")) {

				std::vector<std::string> effects;

				// Populate the effects
				try {
					for (const auto& effect : this->data["nanoleaf"]["effects"]) {
						effects.push_back(effect.get<std::string>());
					}
				}
				catch (const std::exception& e) {
					LOG("Error while loading effects: {}", e.what());
					return;
				}

				if (ImGui::Button("Get Effects List")) {
					RLNanoLeaf::GetEffects([this](const std::string& result) {});
				}

				ImGui::SameLine();

				// Prepare the effects list as a single string for display
				std::string effectsList;
				for (const auto& effect : effects) {
					effectsList += effect + "\n"; // Add each effect to a string
				}

				// Display the effects list
				if (!effectsList.empty()) {
					ImGui::InputTextMultiline("##EffectsList", &effectsList[0], effectsList.size(), ImVec2(300, 100), ImGuiInputTextFlags_ReadOnly);
				}
				else {
					ImGui::Text("No effects fetched yet.");
				}

				// Display popups for each CVar
				DisplayEffectPopup("Main Menu Effect", mainmenuEffectVar, effects);
				DisplayEffectPopup("Overtime Effect", overtimeEffectVar, effects);
				DisplayEffectPopup("Freeplay Effect", freeplayEffectVar, effects);

				if (ImGui::Checkbox("Turn off NanoLeaf on Exit", &exitOff)) {
					exitOffCvar.setValue(exitOff);
				}
				if (!exitOff) {
					DisplayEffectPopup("Exit Effect", exitEffectVar, effects);
				}

				ImGui::TreePop();
			}
		}






		ImGui::Spacing();
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
		if (ImGui::CollapsingHeader("Enable Configuration Items"))
		{
			ImGui::PushItemWidth(33.0f * ImGui::GetFontSize());
			//Team color hook Gui
			bool teamsEnabled = teamsEnableCvar.getBoolValue();


			if (ImGui::Checkbox("Enable Team Colors Effect", &teamsEnabled)) {
				teamsEnableCvar.setValue(teamsEnabled);
			}
			if (ImGui::IsItemHovered()) {
				ImGui::SetTooltip("Toggle Effect");
			}


			//Goal Scored hook Gui

			bool goalScoredEnabled = goalScoredEnableCvar.getBoolValue();

			if (ImGui::Checkbox("Enable Goal Scored Effect", &goalScoredEnabled)) {
				goalScoredEnableCvar.setValue(goalScoredEnabled);
			}
			if (ImGui::IsItemHovered()) {
				ImGui::SetTooltip("Toggle Effect");
			}

			//Demos hook Gui

			bool demosEnabled = demosEnableCvar.getBoolValue();

			if (ImGui::Checkbox("Enable Demos Effect", &demosEnabled)) {
				demosEnableCvar.setValue(demosEnabled);
			}
			if (ImGui::IsItemHovered()) {
				ImGui::SetTooltip("Toggle Effect");
			}



			//Overtime hook GUI

			bool overtimeEnabled = overtimeEnableCvar.getBoolValue();

			if (ImGui::Checkbox("Enable Overtime Effect", &overtimeEnabled)) {
				overtimeEnableCvar.setValue(overtimeEnabled);
			}
			if (ImGui::IsItemHovered()) {
				ImGui::SetTooltip("Toggle Effect");
			}



			//Freeplay hook Gui

			bool freeplayEnabled = freeplayEnableCvar.getBoolValue();

			if (ImGui::Checkbox("Enable Freeplay Effect", &freeplayEnabled)) {
				freeplayEnableCvar.setValue(freeplayEnabled);
			}
			if (ImGui::IsItemHovered()) {
				ImGui::SetTooltip("Toggle Effect");
			}



			//Main Menu hook Gui

			bool mainmenuEnabled = mainmenuEnableCvar.getBoolValue();

			if (ImGui::Checkbox("Enable Main Menu Effect", &mainmenuEnabled)) {
				mainmenuEnableCvar.setValue(mainmenuEnabled);
			}
			if (ImGui::IsItemHovered()) {
				ImGui::SetTooltip("Toggle Effect");
			}



			//matchCountdownEnableCvar hook Gui

			bool matchCountdownEnabled = matchCountdownEnableCvar.getBoolValue();

			if (ImGui::Checkbox("Enable Match Countdown Effect", &matchCountdownEnabled)) {
				matchCountdownEnableCvar.setValue(matchCountdownEnabled);
			}
			if (ImGui::IsItemHovered()) {
				ImGui::SetTooltip("Toggle Effect");
			}



			//Exit hook Gui

			bool exitEnabled = exitEnableCvar.getBoolValue();

			if (ImGui::Checkbox("Enable Game Exit Effect", &exitEnabled)) {
				exitEnableCvar.setValue(exitEnabled);
			}
			if (ImGui::IsItemHovered()) {
				ImGui::SetTooltip("Toggle Effect");
			}

			//Enable Logging
			//
			//bool loggingEnabled = loggingEnableCvar.getBoolValue();
			//
			//if (ImGui::Checkbox("Enable Logging", &loggingEnabled)) {
			//	loggingEnableCvar.setValue(loggingEnabled);
			//}
			//if (ImGui::IsItemHovered()) {
			//	ImGui::SetTooltip("Toggle Logging");
			//}

		}
	}


	ImGui::Spacing();

	ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);

	ImGui::Spacing();


	ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.2f, 1.0f), "Made by GTT1229"); ImGui::SameLine(); ImGui::TextColored(ImVec4(0.2f, 0.6f, 1.0f, 1.0f), "Sponsored by Skordy");

	if (ImGui::TreeNode("Extras")) {

		if (ImGui::Checkbox("Hide IP and Token", &hideURL)) {
			hideURLCvar.setValue(hideURL);
		}
		if (ImGui::IsItemHovered()) {
			ImGui::SetTooltip("Hide IP and Token");
		}

		ImGui::TreePop();

	}


	ImGui::Text("Version:"); ImGui::SameLine(); ImGui::Text(plugin_version);
}




// Do ImGui rendering here
void RLNanoLeaf::Render()
{
	if (!ImGui::Begin(menuTitle_.c_str(), &isWindowOpen_, ImGuiWindowFlags_None))
	{
		// Early out if the window is collapsed, as an optimization.
		ImGui::End();
		return;
	}

	ImGui::End();

	if (!isWindowOpen_)
	{
		cvarManager->executeCommand("togglemenu " + GetMenuName());
	}
}

// Name of the menu that is used to toggle the window.
std::string RLNanoLeaf::GetMenuName()
{
	return "RLNanoLeaf";
}

// Title to give the menu
std::string RLNanoLeaf::GetMenuTitle()
{
	return menuTitle_;
}

// Don't call this yourself, BM will call this function with a pointer to the current ImGui context
//void RLNanoLeaf::SetImGuiContext(uintptr_t ctx)
//{
//	ImGui::SetCurrentContext(reinterpret_cast<ImGuiContext*>(ctx));
//}

// Should events such as mouse clicks/key inputs be blocked so they won't reach the game
bool RLNanoLeaf::ShouldBlockInput()
{
	return ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantCaptureKeyboard;
}

// Return true if window should be interactive
bool RLNanoLeaf::IsActiveOverlay()
{
	return true;
}

// Called when window is opened
void RLNanoLeaf::OnOpen()
{
	isWindowOpen_ = true;

}

// Called when window is closed
void RLNanoLeaf::OnClose()
{
	isWindowOpen_ = false;

}

