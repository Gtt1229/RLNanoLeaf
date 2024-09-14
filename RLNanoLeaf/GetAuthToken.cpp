#include "pch.h"
#include "RLNanoLeaf.h"
#include "IMGUI/imgui_internal.h"
#include "IMGUI/imgui_searchablecombo.h"
#include "IMGUI/imgui_stdlib.h"

void RLNanoLeaf::GetAuthToken() {

	LOG("SendingToken Request");
	//Get NanoLeaf IP
	auto nanoLeafIP = cvarManager->getCvar("cl_rln_nanoLeafIP");
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


	std::string delimiter = R"T("auth_token":")T";
	std::string versionOn = token.erase(0, token.find(delimiter) + delimiter.length());
	std::string delimiter2 = R"T(""")T";
	std::string version = versionOn.substr(0, versionOn.find(delimiter2));
	nanoLeafToken.setValue(token);
	
}