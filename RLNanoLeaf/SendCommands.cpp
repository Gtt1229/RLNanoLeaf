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

	////Get Linear Colors
	//CVarWrapper haMyTeamPrimaryColorCVar = cvarManager->getCvar("cl_rln_myTeamPrimaryRGBColor");
	//if (!haMyTeamPrimaryColorCVar) { return; }
	//CVarWrapper haOtherTeamPrimaryColorCVar = cvarManager->getCvar("cl_rln_OtherTeamPrimaryRGBColor");
	//if (!haOtherTeamPrimaryColorCVar) { return; }
	//
	//
	//LinearColor myTeamPrimaryRGBColor = haMyTeamPrimaryColorCVar.getColorValue();
	//
	//LinearColor OtherTeamPrimaryRGBColor = haOtherTeamPrimaryColorCVar.getColorValue();
	//
	//
	//
	//RLNanoLeaf::RGB myrgb = { myTeamPrimaryRGBColor.R, myTeamPrimaryRGBColor.G, myTeamPrimaryRGBColor.B };
	//
	//
	//RLNanoLeaf::HSV myhsv = rgb2hsv(myrgb);
	//
	//
	//LOG("Hue: {}", myhsv.h);
	//LOG("Sat: {}", myhsv.s);
	//LOG("Bright: {}", myhsv.v);
	//
	//std::string myhue = std::to_string(myhsv.h);
	//std::string mysat = std::to_string(myhsv.s);
	//std::string mybright = std::to_string(myhsv.v);

	RLNanoLeaf::RGB rgb = { color.R, color.G, color.B };


	RLNanoLeaf::HSV hsv = rgb2hsv(rgb);

	std::string hue = std::to_string(hsv.h);
	std::string sat = std::to_string(hsv.s);
	std::string bright = std::to_string(hsv.v);

	//std::string playerScoreString = "0";
	//std::string otherScoreString = "0";
	//int playerScore = 0;
	//int otherScore = 0;

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



	/*if (event == "test")
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
		std::string bright = std::to_string(hsv.v);
	
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
	*/

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
	)T";;


	LOG("RLNanoLeaf Sent:{}", jsonBody);
	req.verb = "PUT";

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
}