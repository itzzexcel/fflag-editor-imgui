#include "Drawing.h"
#include "fflags.hpp"
#include <fstream>

LPCSTR Drawing::lpWindowName = "fflag-wrapper example";
ImVec2 Drawing::vWindowSize = { 600, 400 };
ImGuiWindowFlags Drawing::WindowFlags = { NULL };
bool Drawing::bDraw = true;

void Drawing::Active() { bDraw = true; }
bool Drawing::isActive() { return bDraw; }


char keyBuffer[0x2FA];
char valueBuffer[0x2FAF0];

char newKeyBuffer[0x2FA] = "";
char newValueBuffer[0x2FAF0] = "";

int selectedRow = -1;

void Drawing::Draw() {
    if (!isActive()) return;

    ImGui::SetNextWindowSize(vWindowSize, ImGuiCond_Once);
    ImGui::SetNextWindowBgAlpha(1.0f);

    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 2.5f);

    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.1f, 0.1f, 0.1f, 1));
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.2f, 0.2f, 0.2f, 1));
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.2f, 0.2f, 0.2f, 1));

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f, 0.1f, 0.1f, 1));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.25f, 0.25f, 0.25f, 1));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2f, 0.2f, 0.2f, 1));

    ImGui::PushStyleColor(ImGuiCol_BorderShadow, ImVec4(0.1f, 0.1f, 0.1f, 1));
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.2f, 0.2f, 0.2f, 1));

    ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(0.05f, 0.05f, 0.05f, 1));
    ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(0.1f, 0.1f, 0.1f, 1));
    ImGui::PushStyleColor(ImGuiCol_TitleBgCollapsed, ImVec4(0.05f, 0.05f, 0.05f, 1));

    ImGui::PushStyleColor(ImGuiCol_ResizeGrip, ImVec4(0.1f, 0.1f, 0.1f, 1));
    ImGui::PushStyleColor(ImGuiCol_ResizeGripActive, ImVec4(0.2f, 0.2f, 0.2f, 1));
    ImGui::PushStyleColor(ImGuiCol_ResizeGripHovered, ImVec4(0.2f, 0.2f, 0.2f, 1));

    ImGui::SetNextWindowSizeConstraints(ImVec2(300, 200), ImVec2(FLT_MAX, FLT_MAX));

    ImGui::Begin(lpWindowName, &bDraw, WindowFlags);
    {
        ImGui::Checkbox("Allow Bloxstrap", &FFlags::ALLOW_BLOXSTRAP);

        json jsonContent;
        try {
            std::ifstream input(FFlags::__GetRobloxFolder() + "\\ClientAppSettings.json");
            if (!input.is_open()) {
                ImGui::TextWrapped("Couldn't open the JSON file.");
                ImGui::End();
                return;
            }
            input >> jsonContent;
            input.close();
        }
        catch (const json::parse_error& e) {
            ImGui::TextWrapped("Couldn't parse the JSON file: %s", e.what());
            ImGui::End();
            return;
        }

        if (jsonContent.empty()) {
            ImGui::TextWrapped("The JSON file is empty.");
            ImGui::End();
            return;
        }

        ImGui::BeginChild("", ImGui::GetContentRegionAvail());
        {
            if (!FFlags::__GetRobloxFolder().empty()) {
                ImGui::TextWrapped(std::format("Reading from {}", FFlags::__GetRobloxFolder() + "\\ClientAppSettings.json").c_str());

                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 5.0f);
                if (ImGui::BeginTable("Table", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
                    ImGui::TableSetupColumn("Property");
                    ImGui::TableSetupColumn("Value");
                    ImGui::TableHeadersRow();

                    int index = 0;
                    std::vector<std::string> tempKeys;
                    std::vector<std::string> tempValues;

                    for (auto& item : jsonContent.items()) {
                        tempKeys.push_back(item.key());
                        if (item.value().is_string()) {
                            tempValues.push_back(item.value().get<std::string>());
                        }
                        else {
                            tempValues.push_back(item.value().dump());
                        }
                    }

                    for (size_t i = 0; i < tempKeys.size(); i++) {
                        ImGui::TableNextRow();
                        ImGui::TableNextColumn();

                        strncpy_s(keyBuffer, tempKeys[i].c_str(), sizeof(keyBuffer));
                        keyBuffer[sizeof(keyBuffer) - 1] = '\0';

                        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0, 0, 0, 0));

                        if (ImGui::InputText(("##key" + std::to_string(i)).c_str(), keyBuffer, sizeof(keyBuffer))) {
                            std::string newKey(keyBuffer);

                            if (newKey != tempKeys[i]) {
                                FFlags::Delete(tempKeys[i]);
                                tempKeys[i] = newKey;
                            }
                            FFlags::Write(tempKeys[i], tempValues[i]);
                        }
                        ImGui::PopStyleColor();

                        ImGui::TableNextColumn();

                        strncpy_s(valueBuffer, tempValues[i].c_str(), sizeof(valueBuffer));
                        valueBuffer[sizeof(valueBuffer) - 1] = '\0';

                        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0, 0, 0, 0));
                        if (ImGui::InputText(("##value" + std::to_string(i)).c_str(), valueBuffer, sizeof(valueBuffer))) {
                            tempValues[i] = std::string(valueBuffer);
                            FFlags::Write(std::string(keyBuffer), valueBuffer);
                        }
                        ImGui::PopStyleColor();

                        if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                            selectedRow = i;
                        }

                        if (selectedRow == i) {
                            ImGui::SetItemDefaultFocus();
                            ImGui::TableSetBgColor(ImGuiCol_FrameBg, ImGui::GetColorU32(ImGuiCol_HeaderHovered), true);
                        }
                    }

                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0, 0, 0, 0));
                    ImGui::InputTextWithHint("##x", "Property", newKeyBuffer, sizeof(newKeyBuffer));
                    ImGui::TableNextColumn();

                    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                    ImGui::InputTextWithHint("##y", "Value", newValueBuffer, sizeof(newValueBuffer));

                    ImGui::PopStyleColor();
                    ImGui::EndTable();

                    if (ImGui::Button("Add", ImVec2(ImGui::GetContentRegionAvail().x, 20.0f))) {
                        FFlags::Write(newKeyBuffer, newValueBuffer);
                    }

                    if (ImGui::Button("Delete Selected", ImVec2(ImGui::GetContentRegionAvail().x, 20.0f)) && selectedRow != -1) {
                        FFlags::Delete(tempKeys[selectedRow]);
                    }
                }
            }

        }
    }
}