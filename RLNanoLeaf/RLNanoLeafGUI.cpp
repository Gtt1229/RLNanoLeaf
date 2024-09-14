#include "pch.h"
#include "RLNanoLeaf.h"
#include "IMGUI/imgui_internal.h"
#include "IMGUI/imgui_searchablecombo.h"
#include "IMGUI/imgui_stdlib.h"



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

	//Get URL related Cvars
	CVarWrapper enableCvar = cvarManager->getCvar("cl_rln_enabled");


	CVarWrapper teamsEnableCvar = cvarManager->getCvar("cl_rln_teams_enabled");

	CVarWrapper goalScoredEnableCvar = cvarManager->getCvar("cl_rln_goalScored_enabled");

	CVarWrapper mainmenuEnableCvar = cvarManager->getCvar("cl_rln_mainmenu_enabled");

	CVarWrapper matchCountdownEnableCvar = cvarManager->getCvar("cl_rln_matchCountdown_enabled");

	CVarWrapper demosEnableCvar = cvarManager->getCvar("cl_rln_demos_enabled");

	CVarWrapper overtimeEnableCvar = cvarManager->getCvar("cl_rln_overtime_enabled");

	CVarWrapper freeplayEnableCvar = cvarManager->getCvar("cl_rln_freeplay_enabled");

	CVarWrapper exitEnableCvar = cvarManager->getCvar("cl_rln_exit_enabled");

	CVarWrapper nanoLeafIPCvar = cvarManager->getCvar("cl_rln_nanoLeafIP");
	CVarWrapper nanoLeafTokenCvar = cvarManager->getCvar("cl_rln_nanoLeaftoken");
	CVarWrapper hideURLCvar = cvarManager->getCvar("hideURL");


	CVarWrapper version7Cvar = cvarManager->getCvar("version7");
	if (!version7Cvar) { return; }

	bool version7 = version7Cvar.getBoolValue();

	if (!enableCvar) { return; }
	if (!teamsEnableCvar) { return; }
	if (!goalScoredEnableCvar) { return; }
	if (!overtimeEnableCvar) { return; }
	if (!demosEnableCvar) { return; }
	if (!freeplayEnableCvar) { return; }
	if (!mainmenuEnableCvar) { return; }
	if (!matchCountdownEnableCvar) { return; }




	bool enabled = enableCvar.getBoolValue();
	bool hideURL = hideURLCvar.getBoolValue();
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

		if (ImGui::InputText("NanoLeaf Token", &nanoLeafTokenex)) {

			nanoLeafTokenCvar.setValue(nanoLeafTokenex);

		}


		ImGui::Spacing();
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
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



	}


	ImGui::Spacing();

	ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);

	ImGui::Spacing();


	ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.2f, 1.0f), "Made by GTT1229"); ImGui::SameLine(); ImGui::TextColored(ImVec4(0.2f, 0.6f, 1.0f, 1.0f), "Sponsored by Skordy");

	if (ImGui::TreeNode("Extras")) {

		if (ImGui::Checkbox("Hide JSON URL", &hideURL)) {
			hideURLCvar.setValue(hideURL);
		}
		if (ImGui::IsItemHovered()) {
			ImGui::SetTooltip("Hide JSON URL");
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

