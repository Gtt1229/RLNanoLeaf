#include "pch.h"
#include "nlohmann/json.hpp"
#include "RLNanoLeaf.h"
#include "IMGUI/imgui_internal.h"
#include "IMGUI/imgui_searchablecombo.h"
#include "IMGUI/imgui_stdlib.h"

void RLNanoLeaf::GetAuthToken() {

	LOG("SendingToken Request");
	//Get NanoLeaf IP
	CVarWrapper nanoLeafIP = cvarManager->getCvar("cl_rln_nanoLeafIP");
	if (!nanoLeafIP) { LOG("nanoLeafIP Check Failed"); return; }
	std::string nanoLeafIPex = nanoLeafIP.getStringValue();
	LOG("IP: {}", nanoLeafIPex);

	//Ready NanoLeafToken
	CVarWrapper nanoLeafToken = cvarManager->getCvar("cl_rln_nanoLeaftoken");
	if (!nanoLeafToken) { LOG("nanoLeafToken Check Failed"); return; }

	//JSON formatting
	CurlRequest req;


	req.verb = "POST";




	std::string urlFormatted = "http://"+nanoLeafIPex+":16021/api/v1/new";
	req.url = urlFormatted;
	LOG("URL: {}", urlFormatted);

	std::map<std::string, std::string> jsonHeader{ {"Content-Type", "application/json"} };

	req.headers = jsonHeader;

	req.body = "";


	HttpWrapper::SendCurlRequest(req, [this](int code, std::string result)
		{
			LOG("Body result(Generally empty): {}", result);
			RLNanoLeaf::SetAuthToken(result);
		});

}

void RLNanoLeaf::SetAuthToken(std::string token) {

	//Ready NanoLeafToken
	CVarWrapper nanoLeafToken = cvarManager->getCvar("cl_rln_nanoLeaftoken");
	if (!nanoLeafToken) { LOG("nanoLeafToken Check Failed"); return; }

	//add brackets to jsonReply, since it doesn't come back bracketed for some reason
	std::string jsonStr = "{" + token + "}";

	//// Parse the JSON
	try {
		// Parse the JSON string
		nlohmann::json jsonObj = nlohmann::json::parse(jsonStr);

		// Extract token
		std::string authToken = jsonObj["auth_token"];

		LOG("nanoLeafToken: {}", authToken);

		//Set token CVar
		nanoLeafToken.setValue(authToken);

	}
	catch (nlohmann::json::parse_error& e) {
		// Handle the parsing error
		LOG("Invalid JSON");
		return;
	}
}