#include "pch.h"
#include "RLNanoLeaf.h"
#include "IMGUI/imgui_internal.h"
#include "IMGUI/imgui_searchablecombo.h"
#include "IMGUI/imgui_stdlib.h"




void RLNanoLeaf::SendCommands(std::string event, LinearColor color)
{


	CVarWrapper nanoLeafIP = cvarManager->getCvar("cl_rln_nanoLeafIP");
	if (!nanoLeafIP) { LOG("nanoLeafIP Check Failed"); return; }
	std::string nanoLeafIPex = nanoLeafIP.getStringValue();


	CVarWrapper nanoLeafToken = cvarManager->getCvar("cl_rln_nanoLeaftoken");
	if (!nanoLeafToken) { return; }
	std::string nanoLeafTokenex = nanoLeafToken.getStringValue();

	CVarWrapper panelIDsCvar = cvarManager->getCvar("cl_rln_panelIDs");
	if (!panelIDsCvar) { return; }
	std::string panelIDsEx = panelIDsCvar.getStringValue();

	CVarWrapper brightnessVar = cvarManager->getCvar("cl_rln_brightness");
	if (!brightnessVar) { return; }
	int brightnessAdjust = brightnessVar.getIntValue();
	LOG("{}", brightnessAdjust);


	CVarWrapper exitOffVar = cvarManager->getCvar("cl_rln_exit_off");
	if (!exitOffVar) { return; }

	RLNanoLeaf::RGB rgb = { color.R, color.G, color.B };



	RLNanoLeaf::HSV hsv = rgb2hsv(rgb);
	std::string hue = std::to_string(hsv.h);
	std::string sat = std::to_string(hsv.s);

	//use brightness slider
	int brightInt = brightnessAdjust;
	std::string bright = std::to_string(brightInt);


	if (!gameWrapper->IsInFreeplay()) {
		//Current team CVAR
	//get current team cvar
		CVarWrapper ha_playersTeam = cvarManager->getCvar("ha_playersTeam");
		if (!ha_playersTeam) { return; }
		//
		auto haplayersTeam = cvarManager->getCvar("ha_playersTeam");
		int haplayersTeam2 = haplayersTeam.getIntValue();

		//get current otherteam cvar
		CVarWrapper ha_otherTeam = cvarManager->getCvar("ha_otherTeam");
		if (!ha_otherTeam) { return; }
		//
		auto haotherTeam = cvarManager->getCvar("ha_otherTeam");
		int haotherTeam2 = haotherTeam.getIntValue();

		//LOG("Team nums:{}", haplayersTeam2);
		//LOG("Team nums:{}", haotherTeam2);
		//playerScore = RLNanoLeaf::GetScore(haplayersTeam2);
		//otherScore = RLNanoLeaf::GetScore(haotherTeam2);
		//
		//playerScoreString = std::to_string(playerScore);
		//otherScoreString = std::to_string(otherScore);
	}



	if (event == "test")
	{
	
		CVarWrapper demoColorVar = cvarManager->getCvar("cl_rln_demo_color");
		if (!demoColorVar) { return; }
		LinearColor demoColor = demoColorVar.getColorValue();
		
	
		RLNanoLeaf::RGB rgb = { demoColor.R, demoColor.G, demoColor.B };
	
	
		RLNanoLeaf::HSV hsv = rgb2hsv(rgb);
	
	
		LOG("Hue: {}", hsv.h);
		LOG("Sat: {}", hsv.s);
		LOG("Bright: {}", hsv.v);
	
		std::string hue = std::to_string(hsv.h);
		std::string sat = std::to_string(hsv.s);
		int brightInt = brightnessAdjust;
		std::string bright = std::to_string(brightInt);
	
		LOG("Sending test");
		std::string jsonBody = R"T(
		{
		    "brightness": {
		        "value": )T" + bright + R"T(
		    },
		    "hue": {
		        "value": )T" + hue + R"T(
		    },
		    "sat": {
		        "value": )T" + sat + R"T(
		    }
		}
		)T";
	
		CurlRequest req;
	
		LOG("Sent:{}", jsonBody);
		req.verb = "PUT";
	
	
		auto reqUrlJSON = cvarManager->getCvar("ha_jsonURL");
		std::string reqUrlJSONString = reqUrlJSON.getStringValue();
	
		std::string urlFormatted = "http://" + nanoLeafIPex + ":16021/api/v1/" + nanoLeafTokenex + "/state";
		req.url = urlFormatted;
		LOG("URL:{}", urlFormatted);
		std::map<std::string, std::string> jsonHeader{ {"Content-Type", "application/json"} };
	
		req.headers = jsonHeader;
	
		//LOG("{}", both);
	
		req.body = jsonBody;
	
		HttpWrapper::SendCurlRequest(req, [this](int code, std::string result)
			{
				LOG("Body result(Generally empty): {}", result);
			});
	
		return;
	}

	//JSON formatting
	CurlRequest req;

	std::string jsonBody = R"T(
	{
	    "brightness": {
	        "value": )T" + bright + R"T(
	    },
	    "hue": {
	        "value": )T" + hue + R"T(
	    },
	    "sat": {
	        "value": )T" + sat + R"T(
	    }
	}
	)T";

	bool exitOff = exitOffVar.getBoolValue();




	LOG("RLNanoLeaf Sent:{}", jsonBody);
	req.verb = "PUT";

	std::string urlFormatted = "http://" + nanoLeafIPex + ":16021/api/v1/" + nanoLeafTokenex + "/state";
	req.url = urlFormatted;
	LOG("URL:{}", urlFormatted);
	std::map<std::string, std::string> jsonHeader{ {"Content-Type", "application/json"} };


	if (event == "exit" && exitOff)
	{
		jsonBody = R"T(
		{
		  "on": {
		    "value": false
		  }
		}
		)T";
		req.url = urlFormatted + "/on";
	}



	req.headers = jsonHeader;

	//LOG("{}", both);

	req.body = jsonBody;

	HttpWrapper::SendCurlRequest(req, [this](int code, std::string result)
		{
			LOG("Body result(Generally empty): {}", result);
		});
}

void RLNanoLeaf::SendCommands(std::string effect) {

	CVarWrapper freeplayEffectVar = cvarManager->getCvar("cl_rln_freeplay_effect");
	if (!freeplayEffectVar) { return; }
	std::string freeplayEffect = freeplayEffectVar.getStringValue();

	CVarWrapper overtimeEffectVar = cvarManager->getCvar("cl_rln_overtime_effect");
	if (!overtimeEffectVar) { return; }
	std::string overtimeEffect = overtimeEffectVar.getStringValue();

	CVarWrapper mainmenuEffectVar = cvarManager->getCvar("cl_rln_mainmenu_effect");
	if (!mainmenuEffectVar) { return; }
	std::string mainmenuEffect = mainmenuEffectVar.getStringValue();

	CVarWrapper exitEffectVar = cvarManager->getCvar("cl_rln_exit_effect");
	if (!exitEffectVar) { return; }
	std::string exitEffect = mainmenuEffectVar.getStringValue();



	CVarWrapper nanoLeafIP = cvarManager->getCvar("cl_rln_nanoLeafIP");
	if (!nanoLeafIP) { LOG("nanoLeafIP Check Failed"); return; }
	std::string nanoLeafIPex = nanoLeafIP.getStringValue();


	CVarWrapper nanoLeafToken = cvarManager->getCvar("cl_rln_nanoLeaftoken");
	if (!nanoLeafToken) { return; }
	std::string nanoLeafTokenex = nanoLeafToken.getStringValue();

	CVarWrapper panelIDsCvar = cvarManager->getCvar("cl_rln_panelIDs");
	if (!panelIDsCvar) { return; }
	std::string panelIDsEx = panelIDsCvar.getStringValue();

	std::string jsonBody = "";

	if ( effect == "freeplay") {
		jsonBody = R"T({"select" : ")T" + freeplayEffect + R"T("})T";
	}

	if (effect == "overtime") {
		jsonBody = R"T({"select" : ")T" + mainmenuEffect + R"T("})T";
	}

	if (effect == "mainmenu") {
		jsonBody = R"T({"select" : ")T" + overtimeEffect + R"T("})T";
	}

	if (effect == "exit") {
		jsonBody = R"T({"select" : ")T" + exitEffect + R"T("})T";
	}

	//JSON formatting
	CurlRequest req;

	LOG("RLNanoLeaf Sent:{}", jsonBody);
	req.verb = "PUT";

	std::string urlFormatted = "http://" + nanoLeafIPex + ":16021/api/v1/" + nanoLeafTokenex + "/effects";
	req.url = urlFormatted;
	LOG("URL:{}", urlFormatted);
	std::map<std::string, std::string> jsonHeader{ {"Content-Type", "text/plain"} };

	req.headers = jsonHeader;

	//LOG("{}", both);

	req.body = jsonBody;

	HttpWrapper::SendCurlRequest(req, [this](int code, std::string result)
		{
			LOG("Body result(Generally empty): {}", result);
		});

}