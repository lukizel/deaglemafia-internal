#include <fstream>
#include <functional>
#include <string>
#include <ShlObj.h>
#include <Windows.h>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_stdlib.h"

#include "imguiCustom.h"

#include "GUI.h"
#include "Config.h"
#include "Hacks/Misc.h"
#include "Hacks/Reportbot.h"
#include "Hacks/SkinChanger.h"
#include "Hacks/Visuals.h"
#include "Hooks.h"
#include "SDK/InputSystem.h"
#include "BASS/bass.h"

constexpr auto windowFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize
| ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;

GUI::GUI() noexcept
{
    ImGui::CreateContext();
    ImGui_ImplWin32_Init(FindWindowW(L"Valve001", NULL));

    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    style.FrameBorderSize = 1.0f;
    style.WindowRounding = 0.0f;
    style.ChildRounding = 0.0f;
    style.FrameRounding = 0.0f;
    style.PopupRounding = 0.0f;
    style.ScrollbarRounding = 0.0f;
    style.GrabRounding = 0.0f;
    style.TabRounding = 0.0f;

    style.ScrollbarSize = 9.0f;

    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr;
    io.LogFilename = nullptr;
    io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;

    if (PWSTR pathToFonts; SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Fonts, 0, nullptr, &pathToFonts))) {
        const std::filesystem::path path{ pathToFonts };
        CoTaskMemFree(pathToFonts);

        static constexpr ImWchar ranges[]{ 0x0020, 0xFFFF, 0 };
        fonts.tahoma = io.Fonts->AddFontFromFileTTF((path / "tahoma.ttf").string().c_str(), 15.0f, nullptr, ranges);
        fonts.segoeui = io.Fonts->AddFontFromFileTTF((path / "segoeui.ttf").string().c_str(), 15.0f, nullptr, ranges);
    }
}


void GUI::render() noexcept
{
    ImGui::Begin("deaglemafia internal project 1998", nullptr, windowFlags | ImGuiWindowFlags_AlwaysAutoResize);
    static int switchTabs = 6;

    if (ImGui::Button("Aimbot", ImVec2(100.0f, 0.0f)))
        switchTabs = 0;
    ImGui::SameLine(0.0, 3.0f);
    if (ImGui::Button("Visuals", ImVec2(100.0f, 0.0f)))
        switchTabs = 1;
    ImGui::SameLine(0.0, 3.0f);
    if (ImGui::Button("Skins", ImVec2(100.0f, 0.0f)))
        switchTabs = 5;
    ImGui::SameLine(0.0, 3.0f);
    if (ImGui::Button("Misc", ImVec2(100.0f, 0.0f)))
        switchTabs = 2;
    ImGui::SameLine(0.0, 3.0f);
    if (ImGui::Button("Config", ImVec2(100.0f, 0.0f)))
        switchTabs = 3;
    ImGui::SameLine(0.0, 3.0f);
    if (ImGui::Button("Exit", ImVec2(100.0f, 0.0f)))
        switchTabs = 4;
    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("Exits the game by crashing it. :)");

    static int currentCategory = 0;
    static int currentItem = 0;

    switch (switchTabs) {
    case 0: {
                static int currentCategory{ 0 };
                ImGui::PushItemWidth(110.0f);
                ImGui::PushID(0);
                ImGui::Combo("", &currentCategory, "all\0pistols\0heavy\0smg\0rifles\0");
                ImGui::PopID();
                ImGui::SameLine();
                static int currentWeapon{ 0 };
                ImGui::PushID(1);

                switch (currentCategory) {
                case 0:
                    currentWeapon = 0;
                    ImGui::NewLine();
                    break;
                case 1: {
                    static int currentPistol{ 0 };
                    static constexpr const char* pistols[]{ "all", "glock", "p2000", "usp", "dualies", "p250", "tec", "57", "cz75", "deagle", "r8" };

                    ImGui::Combo("", &currentPistol, [](void* data, int idx, const char** out_text) {
                        if (config->aimbot[idx ? idx : 35].enabled) {
                            static std::string name;
                            name = pistols[idx];
                            *out_text = name.append(" *").c_str();
                        }
                        else {
                            *out_text = pistols[idx];
                        }
                        return true;
                        }, nullptr, IM_ARRAYSIZE(pistols));

                    currentWeapon = currentPistol ? currentPistol : 35;
                    break;
                }
                case 2: {
                    static int currentHeavy{ 0 };
                    static constexpr const char* heavies[]{ "all", "nova", "xm1014", "sawedoff", "mag7", "m249", "negev" };

                    ImGui::Combo("", &currentHeavy, [](void* data, int idx, const char** out_text) {
                        if (config->aimbot[idx ? idx + 10 : 36].enabled) {
                            static std::string name;
                            name = heavies[idx];
                            *out_text = name.append(" *").c_str();
                        }
                        else {
                            *out_text = heavies[idx];
                        }
                        return true;
                        }, nullptr, IM_ARRAYSIZE(heavies));

                    currentWeapon = currentHeavy ? currentHeavy + 10 : 36;
                    break;
                }
                case 3: {
                    static int currentSmg{ 0 };
                    static constexpr const char* smgs[]{ "all", "mac10", "mp9", "mp7", "mp5", "ump", "p90", "bizon" };

                    ImGui::Combo("", &currentSmg, [](void* data, int idx, const char** out_text) {
                        if (config->aimbot[idx ? idx + 16 : 37].enabled) {
                            static std::string name;
                            name = smgs[idx];
                            *out_text = name.append(" *").c_str();
                        }
                        else {
                            *out_text = smgs[idx];
                        }
                        return true;
                        }, nullptr, IM_ARRAYSIZE(smgs));

                    currentWeapon = currentSmg ? currentSmg + 16 : 37;
                    break;
                }
                case 4: {
                    static int currentRifle{ 0 };
                    static constexpr const char* rifles[]{ "all", "galil", "famas", "ak", "m4", "m4", "ssg", "sg", "aug", "awp", "g3sg1", "scar" };

                    ImGui::Combo("", &currentRifle, [](void* data, int idx, const char** out_text) {
                        if (config->aimbot[idx ? idx + 23 : 38].enabled) {
                            static std::string name;
                            name = rifles[idx];
                            *out_text = name.append(" *").c_str();
                        }
                        else {
                            *out_text = rifles[idx];
                        }
                        return true;
                        }, nullptr, IM_ARRAYSIZE(rifles));

                    currentWeapon = currentRifle ? currentRifle + 23 : 38;
                    break;
                }
                }
                ImGui::PopID();
                ImGui::SameLine();
                ImGui::Checkbox("enabled", &config->aimbot[currentWeapon].enabled);
                ImGui::Separator();
                ImGui::Columns(2, nullptr, false);
                ImGui::SetColumnOffset(1, 220.0f);
                ImGui::SameLine();
                ImGui::PushID(2);
                ImGui::PushItemWidth(70.0f);
                ImGui::Combo("", &config->aimbot[currentWeapon].keyMode, "hold\0toggle\0");
                ImGui::PopItemWidth();
                ImGui::PopID();
                ImGui::Checkbox("flashcheck", &config->aimbot[currentWeapon].ignoreFlash);
                ImGui::Checkbox("smokecheck", &config->aimbot[currentWeapon].ignoreSmoke);
                ImGui::Checkbox("aimlock", &config->aimbot[currentWeapon].aimlock);
                ImGui::Checkbox("silent", &config->aimbot[currentWeapon].silent);
                ImGui::Checkbox("friendly", &config->aimbot[currentWeapon].friendlyFire);
                ImGui::Checkbox("only visible", &config->aimbot[currentWeapon].visibleOnly);
                ImGui::Checkbox("only when scoped", &config->aimbot[currentWeapon].scopedOnly);
                ImGui::Checkbox("killshot", &config->aimbot[currentWeapon].killshot);
                ImGui::Checkbox("inbetween shots", &config->aimbot[currentWeapon].betweenShots);
                ImGui::Checkbox("Auto scope", &config->aimbot[currentWeapon].autoScope);
                ImGui::Combo("bone", &config->aimbot[currentWeapon].bone, "nearest\0damage calc\0head\0neck\0sternum\0chest\0stomach\0pelvis\0");
                ImGui::NextColumn();
                ImGui::PushItemWidth(240.0f);
                ImGui::SliderFloat("max aim inaccuracy", &config->aimbot[currentWeapon].maxAimInaccuracy, 0.0f, 1.0f, "%.5f", 2.0f);
                ImGui::SliderFloat("max shot inaccuracy", &config->aimbot[currentWeapon].maxShotInaccuracy, 0.0f, 1.0f, "%.5f", 2.0f);
                ImGui::SliderFloat("field of view", &config->aimbot[currentWeapon].fov, 0.0f, 255.0f, "%.2f", 2.5f);
                ImGui::SliderFloat("smooth", &config->aimbot[currentWeapon].smooth, 1.0f, 100.0f, "%.2f");
                ImGui::InputInt("minimal damage", &config->aimbot[currentWeapon].minDamage);
                config->aimbot[currentWeapon].minDamage = std::clamp(config->aimbot[currentWeapon].minDamage, 0, 250);
                
                ImGui::Columns(1);
                break;
        break;
    }
    case 1: {

        if (ImGui::ListBoxHeader("##", { 125.0f, 300.0f })) {
            static constexpr const char* players[]{ "all players", "visible players", "occluded players" };

            ImGui::Text("allied");
            ImGui::Indent();
            ImGui::PushID("allied");
            ImGui::PushFont(fonts.segoeui);

            for (int i = 0; i < IM_ARRAYSIZE(players); i++) {
                bool isSelected = currentCategory == 0 && currentItem == i;

                if ((i == 0 || !config->esp.players[0].enabled) && ImGui::Selectable(players[i], isSelected)) {
                    currentItem = i;
                    currentCategory = 0;
                }
            }

            ImGui::PopFont();
            ImGui::PopID();
            ImGui::Unindent();
            ImGui::Text("enemies");
            ImGui::Indent();
            ImGui::PushID("enemies");
            ImGui::PushFont(fonts.segoeui);

            for (int i = 0; i < IM_ARRAYSIZE(players); i++) {
                bool isSelected = currentCategory == 1 && currentItem == i;

                if ((i == 0 || !config->esp.players[3].enabled) && ImGui::Selectable(players[i], isSelected)) {
                    currentItem = i;
                    currentCategory = 1;
                }
            }

            ImGui::PopFont();
            ImGui::PopID();
            ImGui::Unindent();
            if (bool isSelected = currentCategory == 2; ImGui::Selectable("weapons", isSelected))
                currentCategory = 2;

            ImGui::Text("projectile");
            ImGui::Indent();
            ImGui::PushID("projectile");
            ImGui::PushFont(fonts.segoeui);
            static constexpr const char* projectiles[]{ "flash", "he", "breach", "bumpmine", "decoy", "molly", "wh nade", "smoke", "snowball" };

            for (int i = 0; i < IM_ARRAYSIZE(projectiles); i++) {
                bool isSelected = currentCategory == 3 && currentItem == i;

                if (ImGui::Selectable(projectiles[i], isSelected)) {
                    currentItem = i;
                    currentCategory = 3;
                }
            }

            ImGui::PopFont();
            ImGui::PopID();
            ImGui::Unindent();

            ImGui::Text("dz");
            ImGui::Indent();
            ImGui::PushID("dz");
            ImGui::PushFont(fonts.segoeui);
            static constexpr const char* dangerZone[]{ "sentry", "drone", "ca$h", "dufflebag", "pistol case", "smg case", "shotgun case", "explosive case", "tool case", "armor set", "body armor", "helmet", "parachute", "$500 target", "upgrade", "exojump", "ammo", "jammer" };

            for (int i = 0; i < IM_ARRAYSIZE(dangerZone); i++) {
                bool isSelected = currentCategory == 4 && currentItem == i;

                if (ImGui::Selectable(dangerZone[i], isSelected)) {
                    currentItem = i;
                    currentCategory = 4;
                }
            }

            ImGui::PopFont();
            ImGui::PopID();
            ImGui::ListBoxFooter();
        }
        ImGui::SameLine();
        if (ImGui::BeginChild("##child", { 400.0f, 0.0f })) {
            switch (currentCategory) {
            case 1: {
                int selected = currentCategory * 3 + currentItem;
                ImGui::Checkbox("enabled", &config->esp.players[selected].enabled);
                ImGui::SameLine(0.0f, 50.0f);
                ImGui::SetNextItemWidth(85.0f);
                ImGui::InputInt("font", &config->esp.players[selected].font, 1, 294);
                config->esp.players[selected].font = std::clamp(config->esp.players[selected].font, 1, 294);

                ImGui::Separator();

                constexpr auto spacing{ 200.0f };
                ImGuiCustom::colorPicker("snaplines", config->esp.players[selected].snaplines);
                ImGui::SameLine(spacing);
                ImGuiCustom::colorPicker("boxes", config->esp.players[selected].box);
                ImGui::SameLine();
                ImGui::SetNextItemWidth(95.0f);
                ImGui::Combo("", &config->esp.players[selected].boxType, "2D\0""2D corners\0""3D\0""3D corners\0");
                ImGuiCustom::colorPicker("eye tracers", config->esp.players[selected].eyeTraces);
                ImGui::SameLine(spacing);
                ImGuiCustom::colorPicker("hp", config->esp.players[selected].health);
                ImGuiCustom::colorPicker("head dot", config->esp.players[selected].headDot);
                ImGui::SameLine(spacing);
                ImGuiCustom::colorPicker("hp bar", config->esp.players[selected].healthBar);
                ImGuiCustom::colorPicker("nickname", config->esp.players[selected].name);
                ImGui::SameLine(spacing);
                ImGuiCustom::colorPicker("ap", config->esp.players[selected].armor);
                ImGuiCustom::colorPicker("ca$h", config->esp.players[selected].money);
                ImGui::SameLine(spacing);
                ImGuiCustom::colorPicker("ap bar", config->esp.players[selected].armorBar);
                ImGuiCustom::colorPicker("outlines", config->esp.players[selected].outline);
                ImGui::SameLine(spacing);
                ImGuiCustom::colorPicker("distance to player", config->esp.players[selected].distance);
                ImGuiCustom::colorPicker("current gun", config->esp.players[selected].activeWeapon);
                ImGui::SameLine(spacing);
                ImGui::Checkbox("turn off after death", &config->esp.players[selected].deadesp);
                ImGui::SliderFloat("distance limiter", &config->esp.players[selected].maxDistance, 0.0f, 200.0f, "%.2fm");
                break;
            }
            case 2: {
                ImGui::Checkbox("enabled", &config->esp.weapon.enabled);
                ImGui::SameLine(0.0f, 50.0f);
                ImGui::SetNextItemWidth(85.0f);
                ImGui::InputInt("font", &config->esp.weapon.font, 1, 294);
                config->esp.weapon.font = std::clamp(config->esp.weapon.font, 1, 294);

                ImGui::Separator();

                constexpr auto spacing{ 200.0f };
                ImGuiCustom::colorPicker("snaplines", config->esp.weapon.snaplines);
                ImGui::SameLine(spacing);
                ImGuiCustom::colorPicker("boxes", config->esp.weapon.box);
                ImGui::SameLine();
                ImGui::SetNextItemWidth(95.0f);
                ImGui::Combo("", &config->esp.weapon.boxType, "2D\0""2D corners\0""3D\0""3D corners\0");
                ImGuiCustom::colorPicker("nickname", config->esp.weapon.name);
                ImGui::SameLine(spacing);
                ImGuiCustom::colorPicker("outlines", config->esp.weapon.outline);
                ImGuiCustom::colorPicker("distance to player", config->esp.weapon.distance);
                ImGui::SliderFloat("distance limiter", &config->esp.weapon.maxDistance, 0.0f, 200.0f, "%.2fm");
                break;
            }
            case 3: {
                ImGui::Checkbox("enabled", &config->esp.projectiles[currentItem].enabled);
                ImGui::SameLine(0.0f, 50.0f);
                ImGui::SetNextItemWidth(85.0f);
                ImGui::InputInt("font", &config->esp.projectiles[currentItem].font, 1, 294);
                config->esp.projectiles[currentItem].font = std::clamp(config->esp.projectiles[currentItem].font, 1, 294);

                ImGui::Separator();

                constexpr auto spacing{ 200.0f };
                ImGuiCustom::colorPicker("snaplines", config->esp.projectiles[currentItem].snaplines);
                ImGui::SameLine(spacing);
                ImGuiCustom::colorPicker("boxes", config->esp.projectiles[currentItem].box);
                ImGui::SameLine();
                ImGui::SetNextItemWidth(95.0f);
                ImGui::Combo("", &config->esp.projectiles[currentItem].boxType, "2D\0""2D corners\0""3D\0""3D corners\0");
                ImGuiCustom::colorPicker("nickname", config->esp.projectiles[currentItem].name);
                ImGui::SameLine(spacing);
                ImGuiCustom::colorPicker("outlines", config->esp.projectiles[currentItem].outline);
                ImGuiCustom::colorPicker("distance to player", config->esp.projectiles[currentItem].distance);
                ImGui::SliderFloat("distance limiter", &config->esp.projectiles[currentItem].maxDistance, 0.0f, 200.0f, "%.2fm");
                break;
            }
            case 4: {
                int selected = currentItem;
                ImGui::Checkbox("enabled", &config->esp.dangerZone[selected].enabled);
                ImGui::SameLine(0.0f, 50.0f);
                ImGui::SetNextItemWidth(85.0f);
                ImGui::InputInt("font", &config->esp.dangerZone[selected].font, 1, 294);
                config->esp.dangerZone[selected].font = std::clamp(config->esp.dangerZone[selected].font, 1, 294);

                ImGui::Separator();

                constexpr auto spacing{ 200.0f };
                ImGuiCustom::colorPicker("snaplines", config->esp.dangerZone[selected].snaplines);
                ImGui::SameLine(spacing);
                ImGuiCustom::colorPicker("boxes", config->esp.dangerZone[selected].box);
                ImGui::SameLine();
                ImGui::SetNextItemWidth(95.0f);
                ImGui::Combo("", &config->esp.dangerZone[selected].boxType, "2D\0""2D corners\0""3D\0""3D corners\0");
                ImGuiCustom::colorPicker("nickname", config->esp.dangerZone[selected].name);
                ImGui::SameLine(spacing);
                ImGuiCustom::colorPicker("outlins", config->esp.dangerZone[selected].outline);
                ImGuiCustom::colorPicker("distance to player", config->esp.dangerZone[selected].distance);
                ImGui::SliderFloat("distance limiter", &config->esp.dangerZone[selected].maxDistance, 0.0f, 200.0f, "%.2fm");
                break;
            }
            }

            ImGui::EndChild();
        }
        break;
    }
    case 2: {
        ImGui::Columns(2, nullptr, false);
        ImGui::SetColumnOffset(1, 230.0f);

        ImGui::Checkbox("antiafk", &config->misc.antiAfkKick);
        ImGui::Checkbox("bhop", &config->misc.bunnyHop);
        ImGui::Checkbox("strafer", &config->misc.autoStrafe);
        ImGui::Checkbox("no duck stamina", &config->misc.fastDuck);
        ImGui::Checkbox("sniper crosshair", &config->misc.sniperCrosshair);
        ImGui::Checkbox("auto accept", &config->misc.autoAccept);
        ImGui::Checkbox("reveal ranks", &config->misc.revealRanks);
        ImGui::Checkbox("reveal suspect name", &config->misc.revealSuspect);
        ImGuiCustom::colorPicker("watermark", config->misc.watermark);
        ImGui::SliderFloat("force ratio", &config->misc.aspectratio, 0.0f, 5.0f, "%.2f");
        ImGui::NextColumn();
        ImGui::Checkbox("no blur", &config->misc.disablePanoramablur);
        ImGui::Checkbox("set clantag", &config->misc.customClanTag);
        ImGui::Checkbox("clock clantag", &config->misc.clocktag);
        ImGui::SameLine();
        ImGui::PushItemWidth(120.0f);
        ImGui::PushID(0);
        if (ImGui::InputText("", &config->misc.clanTag))
            Misc::updateClanTag(true);
        ImGui::PopID();
        ImGui::Checkbox("fast plant", &config->misc.fastPlant);
        ImGuiCustom::colorPicker("bomb time bar", config->misc.bombTimer);
        ImGui::Checkbox("fast reload", &config->misc.quickReload);
        ImGui::Combo("hitsounds", &config->misc.hitSound, "None\0Metal\0Gamesense\0Bell\0Glass\0");
        ImGui::SetNextItemWidth(90.0f);
        ImGui::Checkbox("nade prediction", &config->misc.nadePredict);
        ImGui::Checkbox("underground tablet reception (dz)", &config->misc.fixTabletSignal);
        ImGui::SetNextItemWidth(120.0f);
        ImGui::Checkbox("spoof prime", &config->misc.fakePrime);
        //ImGui::Checkbox("radyjko", &config->misc.radyjko);
        break;
    }
    case 3: {
        ImGui::Columns(2, nullptr, false);
        ImGui::SetColumnOffset(1, 170.0f);

        ImGui::PushItemWidth(160.0f);

        auto& configItems = config->getConfigs();
        static int currentConfig = -1;

        if (static_cast<size_t>(currentConfig) >= configItems.size())
            currentConfig = -1;

        static std::string buffer;

        if (ImGui::ListBox("", &currentConfig, [](void* data, int idx, const char** out_text) {
            auto& vector = *static_cast<std::vector<std::string>*>(data);
            *out_text = vector[idx].c_str();
            return true;
            }, &configItems, configItems.size(), 5) && currentConfig != -1)
            buffer = configItems[currentConfig];

            ImGui::PushID(0);
            if (ImGui::InputText("", &buffer, ImGuiInputTextFlags_EnterReturnsTrue)) {
                if (currentConfig != -1)
                    config->rename(currentConfig, buffer.c_str());
            }
            ImGui::PopID();
            ImGui::NextColumn();

            ImGui::PushItemWidth(100.0f);

            if (ImGui::Button("new cfg", { 100.0f, 25.0f }))
                config->add(buffer.c_str());

            if (ImGui::Button("reset cfg", { 100.0f, 25.0f }))
                ImGui::OpenPopup("which?");

            if (ImGui::BeginPopup("which?")) {
                static constexpr const char* names[]{ "all", "aim", "esp", "Skin changer", "Misc", };
                for (int i = 0; i < IM_ARRAYSIZE(names); i++) {
                    if (i == 1) ImGui::Separator();

                    if (ImGui::Selectable(names[i])) {
                        switch (i) {
                        case 0: config->reset(); Misc::updateClanTag(true); SkinChanger::scheduleHudUpdate(); break;
                        case 1: config->aimbot = { }; break;
                        case 2: config->esp = { }; break;
                        case 3: config->skinChanger = { }; SkinChanger::scheduleHudUpdate(); break;
                        case 4: config->misc = { }; Misc::updateClanTag(true); break;
                        }
                    }
                }
                ImGui::EndPopup();
            }
            if (currentConfig != -1) {
                if (ImGui::Button("load", { 100.0f, 25.0f })) {
                    config->load(currentConfig);
                    SkinChanger::scheduleHudUpdate();
                    Misc::updateClanTag(true);
                }
                if (ImGui::Button("save", { 100.0f, 25.0f }))
                    config->save(currentConfig);
                if (ImGui::Button("delete", { 100.0f, 25.0f }))
                    config->remove(currentConfig);
            }
            break;
    }
    case 4: {
        ImGui::Checkbox("this is an 'exploit' to crash the game. you should not be able to read this!", 0);
        break;
    }
    case 5: {
        static auto itemIndex = 0;

        ImGui::PushItemWidth(110.0f);
        ImGui::Combo("##1", &itemIndex, [](void* data, int idx, const char** out_text) {
            *out_text = game_data::weapon_names[idx].name;
            return true;
            }, nullptr, IM_ARRAYSIZE(game_data::weapon_names), 5);
        ImGui::PopItemWidth();

        auto& selected_entry = config->skinChanger[itemIndex];
        selected_entry.itemIdIndex = itemIndex;

        {
            ImGui::SameLine();
            ImGui::Checkbox("enabled", &selected_entry.enabled);
            ImGui::Separator();
            ImGui::Columns(2, nullptr, false);
            ImGui::InputInt("pattern seed", &selected_entry.seed);
            ImGui::InputInt("stattrak", &selected_entry.stat_trak);
            ImGui::SliderFloat("float", &selected_entry.wear, FLT_MIN, 1.f, "%.10f", 5);

            ImGui::Combo("paintkit", &selected_entry.paint_kit_vector_index, [](void* data, int idx, const char** out_text) {
                *out_text = (itemIndex == 1 ? SkinChanger::gloveKits : SkinChanger::skinKits)[idx].name.c_str();
                return true;
                }, nullptr, (itemIndex == 1 ? SkinChanger::gloveKits : SkinChanger::skinKits).size(), 10);

            ImGui::Combo("quality", &selected_entry.entity_quality_vector_index, [](void* data, int idx, const char** out_text) {
                *out_text = game_data::quality_names[idx].name;
                return true;
                }, nullptr, IM_ARRAYSIZE(game_data::quality_names), 5);

            if (itemIndex == 0) {
                ImGui::Combo("knife", &selected_entry.definition_override_vector_index, [](void* data, int idx, const char** out_text) {
                    *out_text = game_data::knife_names[idx].name;
                    return true;
                    }, nullptr, IM_ARRAYSIZE(game_data::knife_names), 5);
            }
            else if (itemIndex == 1) {
                ImGui::Combo("gloves", &selected_entry.definition_override_vector_index, [](void* data, int idx, const char** out_text) {
                    *out_text = game_data::glove_names[idx].name;
                    return true;
                    }, nullptr, IM_ARRAYSIZE(game_data::glove_names), 5);
            }
            else {
                static auto unused_value = 0;
                selected_entry.definition_override_vector_index = 0;
                ImGui::Combo("unavailable", &unused_value, "knife/glove only!\0");
            }

            ImGui::InputText("custom nametag", selected_entry.custom_name, 32);
        }

        ImGui::NextColumn();

        {
            ImGui::PushID("sticker");

            static auto selectedStickerSlot = 0;

            ImGui::PushItemWidth(-1);

            ImGui::ListBox("", &selectedStickerSlot, [](void* data, int idx, const char** out_text) {
                static char elementName[64];
                auto kit_vector_index = config->skinChanger[itemIndex].stickers[idx].kit_vector_index;
                sprintf_s(elementName, "#%d (%s)", idx + 1, SkinChanger::stickerKits[kit_vector_index].name.c_str());
                *out_text = elementName;
                return true;
                }, nullptr, 5, 5);

            ImGui::PopItemWidth();

            auto& selected_sticker = selected_entry.stickers[selectedStickerSlot];

            ImGui::Combo("stickerkit", &selected_sticker.kit_vector_index, [](void* data, int idx, const char** out_text) {
                *out_text = SkinChanger::stickerKits[idx].name.c_str();
                return true;
                }, nullptr, SkinChanger::stickerKits.size(), 10);

            ImGui::SliderFloat("scratch float", &selected_sticker.wear, FLT_MIN, 1.0f, "%.10f", 5.0f);
            ImGui::SliderFloat("scale", &selected_sticker.scale, 0.1f, 5.0f);
            ImGui::SliderFloat("rotation", &selected_sticker.rotation, 0.0f, 360.0f);

            ImGui::PopID();
        }
        selected_entry.update();

        ImGui::Columns(1);

        ImGui::Separator();

        if (ImGui::Button("force update", { 130.0f, 30.0f }))
            SkinChanger::scheduleHudUpdate();
        ImGui::SameLine(0.0, 10.0f);
        ImGui::TextUnformatted("code based on nskinz by namazso - github namazso/nSkinz");
        break;
        }
    }
    ImGui::End();
}

