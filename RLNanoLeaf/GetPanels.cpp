#include "pch.h"
#include <nlohmann/json.hpp>
#include "RLNanoLeaf.h"
#include "IMGUI/imgui_internal.h"
#include "IMGUI/imgui_searchablecombo.h"
#include "IMGUI/imgui_stdlib.h"
#include <filesystem>
#include <fstream>

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





void RLNanoLeaf::GetEffects(GetEffectsCallback callback) {

    this->dataDir = this->gameWrapper->GetDataFolder().append("RLNanoLeaf");
    this->mainPath = std::filesystem::path(dataDir).append(this->mainFile);
    this->tmpPath = std::filesystem::path(dataDir).append(this->tmpFile);
    this->bakPath = std::filesystem::path(dataDir).append(this->bakFile);
    CVarWrapper nanoLeafIP = cvarManager->getCvar("cl_rln_nanoLeafIP");
    if (!nanoLeafIP) { LOG("nanoLeafIP Check Failed"); return; }
    std::string nanoLeafIPex = nanoLeafIP.getStringValue();

    CVarWrapper nanoLeafToken = cvarManager->getCvar("cl_rln_nanoLeaftoken");
    if (!nanoLeafToken) { return; }
    std::string nanoLeafTokenex = nanoLeafToken.getStringValue();

    CurlRequest req;
    req.verb = "GET";
    std::string urlFormatted = "http://" + nanoLeafIPex + ":16021/api/v1/" + nanoLeafTokenex + "/effects/effectsList";
    req.url = urlFormatted;
    LOG("URL: {}", urlFormatted);

    std::map<std::string, std::string> jsonHeader{ {"Content-Type", "application/json"} };
    req.headers = jsonHeader;
    req.body = "";

    HttpWrapper::SendCurlRequest(req, [this](int code, std::string result) {
        try {
            LOG("Raw JSON response: {}", result);

            // Parse the JSON response
            nlohmann::json jsonObj = nlohmann::json::parse(result);

            if (jsonObj.is_array()) {
                this->data["nanoleaf"]["effects"] = jsonObj;

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
                            LOG("Could not backup effects data.");
                            return;
                        }
                    }
                    std::filesystem::rename(this->tmpPath, this->mainPath, err);
                    if (err.value() != 0) {
                        LOG("Could not move temp file to main");
                        std::filesystem::rename(this->bakPath, this->mainPath, err);
                    }
                }
                catch (const nlohmann::detail::exception& e) {
                    LOG("Failed to serialize JSON");
                }
                catch (...) {
                    LOG("Unknown error occurred during JSON serialization");
                }

                LOG("Effects successfully saved.");
            }
            else {
                LOG("Invalid JSON structure for effects list.");
            }
        }
        catch (nlohmann::json::parse_error& e) {
            LOG("JSON parse error: {}", e.what());
        }
        catch (std::exception& e) {
            LOG("Exception: {}", e.what());
        }
        });

    WriteData();
}