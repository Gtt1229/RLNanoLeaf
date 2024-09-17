#include "pch.h"
#include <nlohmann/json.hpp>
#include "RLNanoLeaf.h"
#include "IMGUI/imgui_internal.h"
#include "IMGUI/imgui_searchablecombo.h"
#include "IMGUI/imgui_stdlib.h"

void RLNanoLeaf::GetPanels() {

	CVarWrapper nanoLeafIP = cvarManager->getCvar("cl_rln_nanoLeafIP");
	if (!nanoLeafIP) { LOG("nanoLeafIP Check Failed"); return; }
	std::string nanoLeafIPex = nanoLeafIP.getStringValue();


	CVarWrapper nanoLeafToken = cvarManager->getCvar("cl_rln_nanoLeaftoken");
	if (!nanoLeafToken) { return; }
	std::string nanoLeafTokenex = nanoLeafToken.getStringValue();

	CVarWrapper panelIDsCvar = cvarManager->getCvar("cl_rln_panelIDs");
	if (!panelIDsCvar) { return; }
	std::string panelIDsEx = panelIDsCvar.getStringValue();
	LOG("PanelIDs Before Request: {}", panelIDsEx);


	CurlRequest req;
	req.verb = "GET";
	std::string urlFormatted = "http://" + nanoLeafIPex + ":16021/api/v1/" + nanoLeafTokenex + "/panelLayout/layout";
	req.url = urlFormatted;
	LOG("URL: {}", urlFormatted);

	std::map<std::string, std::string> jsonHeader{ {"Content-Type", "application/json"} };

	req.headers = jsonHeader;

	req.body = "";


	HttpWrapper::SendCurlRequest(req, [this](int code, std::string result)
		{
			try {
				// Parse the JSON response
				nlohmann::json jsonObj = nlohmann::json::parse(result);
	
				// Vector to store the panel IDs
				std::vector<int> panelIDs;
	
				// Loop through the positionData array and extract panelId
				for (const auto& panel : jsonObj["positionData"]) {
					panelIDs.push_back(panel["panelId"]);
				}
				std::ostringstream oss;
				std::copy(panelIDs.begin(), panelIDs.end(), std::ostream_iterator<int>(oss, " "));
				std::string panelIDsStr = oss.str();
				// Output the panel IDs
				LOG("PanelIDs string: {}", panelIDsStr);

				CVarWrapper panelIDsCvar = cvarManager->getCvar("cl_rln_panelIDs");
				if (!panelIDsCvar) { return; }
				std::string panelIDsEx = panelIDsCvar.getStringValue();
				LOG("PanelIDs after request: {}", panelIDsEx);
				panelIDsCvar.setValue(panelIDsStr);
	
			}
			catch (nlohmann::json::parse_error& e) {
	
			}
		});





}