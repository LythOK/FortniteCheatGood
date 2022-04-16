/*
	Copyright (c) 2022 Chase1803
	Permission is hereby granted, free of charge, to any person
	obtaining a copy of this software and associated documentation
	files (the "Software"), to deal in the Software without
	restriction, including without limitation the rights to use,
	copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the
	Software is furnished to do so, subject to the following
	conditions:
	The above copyright notice and this permission notice shall be
	included in all copies or substantial portions of the Software.
	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
	EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
	OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
	NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
	HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
	WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
	FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
	OTHER DEALINGS IN THE SOFTWARE.
 */

/*
 * Gloomy.cc
 * Made by Chase
 * https://github.com/Chase1803
*/

#include <iostream>
#include "GL/glew.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <vector>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <Windows.h>
#include <TlHelp32.h>
#include <tchar.h>
#include <intrin.h>
#include "driverloader/singed.hpp"
#include"signed.h"
#include "Controller.h"
#include <d3d9.h>
#include "d3dx9.h"
#include <thread>
#include <string>
#include <stdio.h>
#include <fstream>

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

int g_width;
int g_height;
HWND fortnite_wnd;
GLFWwindow* g_window;

extern void aimbot(float x, float y);

int g_pid;
uint64_t g_base_address;
uint64_t pattern_uworld;

#include "font.h"

bool g_overlay_visible{ false };
bool g_fovchanger{ false };
bool g_esp_enabled{ true };
bool g_esp_distance{ true };
bool g_esp_skeleton{ false };
bool g_3d_box{ false };
bool g_aimbot{ true };
bool g_skipknocked{ true };
bool g_trigger{ false };
bool g_lineesp{ false };
bool g_boxesp{ false };
bool g_fov{ false };
bool g_circlefov{ true };
bool g_crossh{ true };
bool g_cornerboxesp{ false };
bool g_chests{ false };
bool g_vehicles{ false };
bool g_ammo{ false };
bool g_ammos{ false };
bool g_loot{ false };
bool g_curweaponesp{ true };
bool g_consumables{ false };
bool g_spoofesp{ false };
bool g_mouse_aim{ true };
bool g_mem_aim{ false };
bool g_utils{ false };
bool controller = false;
bool g_exploits_backtrack{ false };
bool g_gun_tracers{ false };
bool g_disable_gunshots{ false };
bool g_playerfly{ false };
bool g_chams{ false };
bool g_platform_esp{ false };
bool g_name_esp{ false };

#define maxPlayerCount 40
#include "utils.hpp"

std::wstring s2ws(const std::string& str) {
	int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
	std::wstring wstrTo(size_needed, 0);
	MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
	return wstrTo;
}

int get_fn_processid() {
	BYTE target_name[] = { 'F','o','r','t','n','i','t','e','C','l','i','e','n','t','-','W','i','n','6','4','-','S','h','i','p','p','i','n','g','.','e','x','e', 0 };
	std::wstring process_name = s2ws(std::string((char*)target_name));
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32W entry;
	entry.dwSize = sizeof(entry);

	if (!Process32First(snapshot, &entry)) {
		return 0;
	}

	while (Process32Next(snapshot, &entry)) {
		if (std::wstring(entry.szExeFile) == process_name) {
			return entry.th32ProcessID;
		}
	}

	return 0;
}

static void glfwErrorCallback(int error, const char* description)
{
	fprintf(stderr, XorStr("OverEW: %d: %s\n").c_str(), error, description);
}

void setupWindow() 
{
	glfwSetErrorCallback(glfwErrorCallback);
	if (!glfwInit()) {
		std::cout << XorStr("Over Init Failed!\n").c_str();
		return;
	}

	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	if (!monitor) {
		fprintf(stderr, XorStr("Failed to get monitor!\n").c_str());
		return;
	}

	g_width = glfwGetVideoMode(monitor)->width;
	g_height = glfwGetVideoMode(monitor)->height;

	glfwWindowHint(GLFW_FLOATING, true);
	glfwWindowHint(GLFW_RESIZABLE, false);
	glfwWindowHint(GLFW_MAXIMIZED, false);
	glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, true);

	g_window = glfwCreateWindow(g_width, g_height, XorStr("xxx????xxxx").c_str(), NULL, NULL);

	if (g_window == NULL) {
		std::cout << XorStr("Over: 0x2!\n").c_str();
		return;
	}

	glfwSetWindowAttrib(g_window, GLFW_DECORATED, false);
	glfwSetWindowAttrib(g_window, GLFW_MOUSE_PASSTHROUGH, true);

	glfwSetWindowMonitor(g_window, NULL, 0, 0, g_width, g_height + 1, 0);

	glfwMakeContextCurrent(g_window);
	//glfwSwapInterval(1); 

	if (glewInit() != GLEW_OK)
	{
		fprintf(stderr, XorStr("Over: 0x1!\n").c_str());
		return;
	}

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	io.IniFilename = NULL;

	ImGuiStyle& s = ImGui::GetStyle();

	io.Fonts->AddFontFromMemoryCompressedTTF(Test_compressed_data, Test_compressed_size, 13.f);

	ImGui::StyleColorsDark();

	const ImColor bgSecondary = ImColor(15, 15, 15, 255);
	s.Colors[ImGuiCol_WindowBg] = ImColor(32, 32, 32, 255);
	s.Colors[ImGuiCol_ChildBg] = bgSecondary;
	s.Colors[ImGuiCol_FrameBg] = ImColor(65, 64, 64, 255);//128,128,128
	s.Colors[ImGuiCol_FrameBgActive] = ImColor(35, 37, 39, 255);
	s.Colors[ImGuiCol_Border] = ImColor(0, 0, 0, 255);
	s.Colors[ImGuiCol_CheckMark] = ImColor(255, 0, 0, 255);
	s.Colors[ImGuiCol_SliderGrab] = ImColor(255, 255, 255, 150);
	s.Colors[ImGuiCol_SliderGrabActive] = ImColor(255, 255, 255, 255);
	s.Colors[ImGuiCol_ResizeGrip] = ImColor(24, 24, 24, 255);
	s.Colors[ImGuiCol_Header] = ImColor(0, 0, 0, 255);
	s.Colors[ImGuiCol_HeaderHovered] = ImColor(0, 0, 0, 255);
	s.Colors[ImGuiCol_HeaderActive] = ImColor(0, 0, 0, 255);
	s.Colors[ImGuiCol_TitleBg] = ImColor(247, 255, 25, 255);
	s.Colors[ImGuiCol_TitleBgCollapsed] = ImColor(247, 255, 25, 255);
	s.Colors[ImGuiCol_TitleBgActive] = ImColor(247, 255, 25, 255);
	s.Colors[ImGuiCol_FrameBgHovered] = ImColor(65, 64, 64, 255);
	s.Colors[ImGuiCol_ScrollbarBg] = ImColor(0, 0, 0, 255);
	s.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(128, 128, 128, 255);
	s.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(255, 255, 255, 255);
	s.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(255, 255, 255, 255);
	s.Colors[ImGuiCol_Header] = ImColor(42, 42, 42, 255);
	s.Colors[ImGuiCol_HeaderHovered] = ImColor(50, 50, 50, 255);
	s.Colors[ImGuiCol_HeaderActive] = ImColor(50, 50, 50, 255);
	s.Colors[ImGuiCol_PopupBg] = ImColor(15, 15, 15, 255);
	s.Colors[ImGuiCol_Button] = ImColor(30, 30, 30, 255);//
	s.Colors[ImGuiCol_ButtonHovered] = ImColor(128, 128, 128, 150);
	s.Colors[ImGuiCol_ButtonActive] = ImColor(128, 128, 128, 255);

	ImGui_ImplGlfw_InitForOpenGL(g_window, true);
	ImGui_ImplOpenGL3_Init(XorStr("#version 130").c_str());
}

void cleanupWindow() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(g_window);
	glfwTerminate();
}

BOOL CALLBACK retreiveFortniteWindow(HWND hwnd, LPARAM lparam) {
	DWORD process_id;
	GetWindowThreadProcessId(hwnd, &process_id);
	if (process_id == g_pid) {
		fortnite_wnd = hwnd;
	}
	return TRUE;
}

bool isFortniteFocused = false;

void activateFortniteWindow() {
	SetForegroundWindow(fortnite_wnd);
}

void handleKeyPresses() 
{
	if (GetAsyncKeyState(VK_F1) & 1) {
		g_overlay_visible = !g_overlay_visible;
		glfwSetWindowAttrib(g_window, GLFW_MOUSE_PASSTHROUGH, !g_overlay_visible);
		if (g_overlay_visible) {
			HWND overlay_window = glfwGetWin32Window(g_window);
			SetForegroundWindow(overlay_window);
		}
		else {
			activateFortniteWindow();
		}
	}
}

std::string string_To_UTF8(const std::string& str)
{
	int nwLen = ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);

	wchar_t* pwBuf = new wchar_t[nwLen + 1];
	ZeroMemory(pwBuf, nwLen * 2 + 2);

	::MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length(), pwBuf, nwLen);

	int nLen = ::WideCharToMultiByte(CP_UTF8, 0, pwBuf, -1, NULL, NULL, NULL, NULL);

	char* pBuf = new char[nLen + 1];
	ZeroMemory(pBuf, nLen + 1);

	::WideCharToMultiByte(CP_UTF8, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);

	std::string retStr(pBuf);

	delete[]pwBuf;
	delete[]pBuf;

	pwBuf = NULL;
	pBuf = NULL;

	return retStr;
}

void DrawText1(int x, int y, const char* str, RGBA* color)
{
	ImFont a;
	std::string utf_8_1 = std::string(str);
	std::string utf_8_2 = string_To_UTF8(utf_8_1);
	ImGui::GetOverlayDrawList()->AddText(ImVec2(x, y), ImGui::ColorConvertFloat4ToU32(ImVec4(color->R / 255.0, color->G / 255.0, color->B / 255.0, color->A / 255.0)), utf_8_2.c_str());
}

void DrawCorneredBox(int X, int Y, int W, int H, const ImU32& color, int thickness) {
	float lineW = (W / 3);
	float lineH = (H / 3);

	//corners
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(X, Y), ImVec2(X, Y + lineH), ImGui::GetColorU32(color), thickness);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(X, Y), ImVec2(X + lineW, Y), ImGui::GetColorU32(color), thickness);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(X + W - lineW, Y), ImVec2(X + W, Y), ImGui::GetColorU32(color), thickness);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(X + W, Y), ImVec2(X + W, Y + lineH), ImGui::GetColorU32(color), thickness);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(X, Y + H - lineH), ImVec2(X, Y + H), ImGui::GetColorU32(color), thickness);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(X, Y + H), ImVec2(X + lineW, Y + H), ImGui::GetColorU32(color), thickness);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(X + W - lineW, Y + H), ImVec2(X + W, Y + H), ImGui::GetColorU32(color), thickness);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(X + W, Y + H - lineH), ImVec2(X + W, Y + H), ImGui::GetColorU32(color), thickness);
}

typedef struct Playertest
{
	float ItemDist;
	uint64_t Acotr;
	uint64_t Acotrmesh;
	DWORD_PTR rootcomp;
	bool isbot;
	bool storebot;
	std::string name;
	wchar_t* LootNames;
	Vector3 ItemPosition;
}Playertest;
std::vector<Playertest> PLIST;

typedef struct _LootEntity {
	ImDrawList* Renderer;
	std::string name;
	uintptr_t CurrentActor;
	Vector3 LocalRelativeLocation;
}LootEntity;
static std::vector<LootEntity> LootentityList;


float closestDistance = FLT_MAX;
DWORD_PTR closestPawn = NULL;
int LocalTeam;
static bool targetlocked = false;

uintptr_t GWorld;
uintptr_t LocalPlayerController;
uint64_t PlayerCameraManager;
bool InLobby = false;

void drawlootloop()
{
	if (Globals::LocalPawn)
	{
		std::vector<LootEntity> tmpList;
		uintptr_t ItemLevels = read<uintptr_t>(g_pid, GWorld + 0x148);

		for (int i = 0; i < read<DWORD>(g_pid, GWorld + (0x148 + sizeof(PVOID))); ++i) {

			uintptr_t ItemLevel = read<uintptr_t>(g_pid, ItemLevels + (i * sizeof(uintptr_t)));

			for (int i = 0; i < read<DWORD>(g_pid, ItemLevel + (0x98 + sizeof(PVOID))); ++i) {


				uintptr_t ItemsPawns = read<uintptr_t>(g_pid, ItemLevel + 0x98);
				uintptr_t CurrentItemPawn = read<uintptr_t>(g_pid, ItemsPawns + (i * sizeof(uintptr_t)));

				uintptr_t ItemRootComponent = read<uintptr_t>(g_pid, CurrentItemPawn + 0x188);
				Vector3 ItemPosition = read<Vector3>(g_pid, ItemRootComponent + 0x128);
				float ItemDist = Globals::LocalPlayerRelativeLocation.Distance(ItemPosition) / 100.f;


				if (ItemDist < bLootRendering) {

					int currentitemid = read<int>(g_pid, CurrentItemPawn + 0x18);
					auto CurrentItemPawnName = GetNameFromFName(currentitemid);

					if ((g_loot && strstr(CurrentItemPawnName.c_str(), (XorStr("FortPickupAthena")).c_str())) || strstr(CurrentItemPawnName.c_str(), (XorStr("Tiered_Chest").c_str())) || 
						(g_vehicles && strstr(CurrentItemPawnName.c_str(), XorStr("Vehicl").c_str()) || strstr(CurrentItemPawnName.c_str(), XorStr("Valet_Taxi").c_str()) || 
						strstr(CurrentItemPawnName.c_str(), XorStr("Valet_BigRig").c_str()) || strstr(CurrentItemPawnName.c_str(), XorStr("Valet_BasicTr").c_str()) || 
						strstr(CurrentItemPawnName.c_str(), XorStr("Valet_SportsC").c_str()) || strstr(CurrentItemPawnName.c_str(), XorStr("Valet_BasicC").c_str()))) 
					{
						LootEntity fnlEntity{ };
						fnlEntity.CurrentActor = CurrentItemPawn;
						fnlEntity.name = CurrentItemPawnName;
						tmpList.push_back(fnlEntity);

					}
				}

			}
		}
		LootentityList.clear();
		LootentityList = tmpList;
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}

void CacheNew()
{
	while (true)
	{
		std::vector<Playertest> tmpList;

		GWorld = read<uintptr_t>(g_pid, pattern_uworld);

		uintptr_t PersistentLevel = read<uintptr_t>(g_pid, GWorld + 0x30);

		uintptr_t GameInstance = read<uintptr_t>(g_pid, GWorld + 0x190);

		uintptr_t LocalPlayers = read<uintptr_t>(g_pid, GameInstance + 0x38);

		Globals::LocalPlayer = read<uintptr_t>(g_pid, LocalPlayers);

		LocalPlayerController = read<uintptr_t>(g_pid, Globals::LocalPlayer + 0x30);

		PlayerCameraManager = read<uint64_t>(g_pid, LocalPlayerController + 0x338);

		Globals::LocalPawn = read<uintptr_t>(g_pid, LocalPlayerController + 0x320);

		Globals::LocalPawnRootComponent = read<uintptr_t>(g_pid, Globals::LocalPawn + 0x188);//0x190

		uint64_t localplayerstate = read<uint64_t>(g_pid, Globals::LocalPawn + 0x2A0);

		LocalTeam = read<int>(g_pid, localplayerstate + 0x1020);

		InLobby = false;
		if (!Globals::LocalPawn) InLobby = true;

		auto ActorCount = read<DWORD>(g_pid, PersistentLevel + 0xA0);

		auto AActors2 = read<uintptr_t>(g_pid, PersistentLevel + 0x98);


		for (int i = 0; i < ActorCount; ++i) {


			uintptr_t CurrentItemPawn = read<uintptr_t>(g_pid, AActors2 + (i * sizeof(uintptr_t)));

			int CurrentItemId = read<int>(g_pid, CurrentItemPawn + 0x18);

			auto CurrentItemPawnName = GetNameFromFName(CurrentItemId);

			if (strstr(CurrentItemPawnName.c_str(), "PlayerPawn_Athena_C") || strstr(CurrentItemPawnName.c_str(), "PlayerPawn"))
			{
				Playertest Actor{ };

				Actor.Acotr = CurrentItemPawn;
				Actor.Acotrmesh = read<uint64_t>(g_pid, CurrentItemPawn + 0x300);
				Actor.name = CurrentItemPawnName;
				Actor.rootcomp = Globals::LocalPawnRootComponent;

				tmpList.push_back(Actor);
			}
		}

		drawlootloop();
		PLIST.clear();
		PLIST = tmpList;
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}

bool isVisible(DWORD_PTR mesh)
{
	if (!mesh)
		return false;
	float fLastSubmitTime = read<float>(g_pid, mesh + 0x330);
	float fLastRenderTimeOnScreen = read<float>(g_pid, mesh + 0x334);

	const float fVisionTick = 0.06f;
	bool bVisible = fLastRenderTimeOnScreen + fVisionTick >= fLastSubmitTime;
	return bVisible;
}


bool IsVec3Valid(Vector3 vec3)
{
	return !(vec3.x == 0 && vec3.y == 0 && vec3.z == 0);
}

static int Depth;

static class CKey {
private:
	BYTE bPrevState[0x100];
public:
	CKey() {
		memset(bPrevState, 0, sizeof(bPrevState));
	}

	BOOL IsKeyPushing(BYTE vKey) {
		return GetAsyncKeyState(vKey) & 0x8000;
	}

	BOOL IsKeyPushed(BYTE vKey) {
		BOOL bReturn = FALSE;

		if (IsKeyPushing(vKey))
			bPrevState[vKey] = TRUE;
		else
		{
			if (bPrevState[vKey] == TRUE)
				bReturn = TRUE;
			bPrevState[vKey] = FALSE;
		}

		return bReturn;
	}
};

static CKey Key;
static bool isaimbotting;
bool isVis;

char* wchar_to_char(const wchar_t* pwchar)
{
	int currentCharIndex = 0;
	char currentChar = pwchar[currentCharIndex];

	while (currentChar != '\0')
	{
		currentCharIndex++;
		currentChar = pwchar[currentCharIndex];
	}

	const int charCount = currentCharIndex + 1;

	char* filePathC = (char*)malloc(sizeof(char) * charCount);

	for (int i = 0; i < charCount; i++)
	{
		char character = pwchar[i];

		*filePathC = character;

		filePathC += sizeof(char);

	}
	filePathC += '\0';

	filePathC -= (sizeof(char) * charCount);

	return filePathC;
}

void DrawLString(float fontSize, int x, int y, ImU32 color, bool bCenter, bool stroke, const char* pText, ...)
{
	va_list va_alist;
	char buf[1024] = { 0 };
	va_start(va_alist, pText);
	_vsnprintf_s(buf, sizeof(buf), pText, va_alist);
	va_end(va_alist);
	std::string text = WStringToUTF8(MBytesToWString(buf).c_str());
	if (bCenter)
	{
		ImVec2 textSize = ImGui::CalcTextSize(text.c_str());
		x = x - textSize.x / 2;
		y = y - textSize.y;
	}
	if (stroke)
	{
		ImGui::GetOverlayDrawList()->AddText(ImGui::GetFont(), fontSize, ImVec2(x + 1, y + 1), ImGui::ColorConvertFloat4ToU32(ImVec4(0, 0, 0, 1)), text.c_str());
		ImGui::GetOverlayDrawList()->AddText(ImGui::GetFont(), fontSize, ImVec2(x - 1, y - 1), ImGui::ColorConvertFloat4ToU32(ImVec4(0, 0, 0, 1)), text.c_str());
		ImGui::GetOverlayDrawList()->AddText(ImGui::GetFont(), fontSize, ImVec2(x + 1, y - 1), ImGui::ColorConvertFloat4ToU32(ImVec4(0, 0, 0, 1)), text.c_str());
		ImGui::GetOverlayDrawList()->AddText(ImGui::GetFont(), fontSize, ImVec2(x - 1, y + 1), ImGui::ColorConvertFloat4ToU32(ImVec4(0, 0, 0, 1)), text.c_str());
	}
	ImGui::GetOverlayDrawList()->AddText(ImGui::GetFont(), fontSize, ImVec2(x, y), color, text.c_str());
}

Vector3 calc_angle(Vector3& zaz, Vector3& daz) {
	Vector3 dalte = zaz - daz;
	Vector3 ongle;
	double hpm = sqrt(dalte.x * dalte.x + dalte.y * dalte.y);
	ongle.y = atan(dalte.y / dalte.x) * 57.295779513082;
	ongle.x = (atan(dalte.z / hpm) * 57.295779513082) * -1;
	if (dalte.x >= 0.f) ongle.y += 180;
	return ongle;
}

void WriteAngles(Vector3 Location)
{
	auto camloc = read<Vector3>(g_pid, Globals::LocalPawnRootComponent + 0x128);

	static auto Pdefaultxmin = read<float>(g_pid, PlayerCameraManager + 0x3184);
	static auto Pdefaultxmax = read<float>(g_pid, PlayerCameraManager + 0x3188);
	static auto Ydefaultxmin = read<float>(g_pid, PlayerCameraManager + 0x318c);
	static auto Ydefaultxmax = read<float>(g_pid, PlayerCameraManager + 0x3190);

	Vector3 angles = calc_angle(camloc, Location);

	write<float>(g_pid, PlayerCameraManager + 0x3184, angles.x + 1.f);
	write<float>(g_pid, PlayerCameraManager + 0x3188, angles.x + 1.f);
	write<float>(g_pid, PlayerCameraManager + 0x318c, angles.y - 2.f);
	write<float>(g_pid, PlayerCameraManager + 0x3190, angles.y - 2.f);
	Sleep(5);
	write<float>(g_pid, PlayerCameraManager + 0x3184, Pdefaultxmin);
	write<float>(g_pid, PlayerCameraManager + 0x3188, Pdefaultxmax);
	write<float>(g_pid, PlayerCameraManager + 0x318c, Ydefaultxmin);
	write<float>(g_pid, PlayerCameraManager + 0x3190, Ydefaultxmax);
}

bool actorLoop() 
{
	bool bValidEnemyInArea = false;
	float ClosestActorDistance = FLT_MAX;
	Vector3 ClosestActorMouseAimbotPosition = Vector3(0.0f, 0.0f, 0.0f);
	float distance, distaim;

	try
	{
		for (LootEntity entity : LootentityList) {

			if (Globals::LocalPawn)
			{
				uintptr_t ItemRootComponent = read<uintptr_t>(g_pid, entity.CurrentActor + 0x188);
				Vector3 ItemPosition = read<Vector3>(g_pid, ItemRootComponent + 0x128);
				float ItemDist = Globals::LocalPlayerRelativeLocation.Distance(ItemPosition) / 100.f;
				std::string null = ("");

				auto IsSearched = read<BYTE>(g_pid, (uintptr_t)entity.CurrentActor + 0xF61);
				if (IsSearched >> 7 & 1) continue;

				if (strstr(entity.name.c_str(), ("Tiered_Chest")) && g_chests && CheckInScreen(entity.CurrentActor, Globals::Width, Globals::Height))
				{
					if (ItemDist < bLootRendering) {
						Vector3 ChestPosition;
						ChestPosition = g_functions::ConvertWorld2Screen(ItemPosition);
						std::string Text = null + ("Chest [") + std::to_string((int)ItemDist) + ("m]");
						DrawString(14, ChestPosition.x, ChestPosition.y, &Col.yellow, true, true, Text.c_str());
					}
				}
				else if ((g_vehicles && (strstr(entity.name.c_str(), XorStr("Vehicl").c_str()) || strstr(entity.name.c_str(), XorStr("Valet_Taxi").c_str()) || strstr(entity.name.c_str(), XorStr("Valet_BigRig").c_str()) || strstr(entity.name.c_str(), XorStr("Valet_BasicTr").c_str()) || strstr(entity.name.c_str(), XorStr("Valet_SportsC").c_str()) || strstr(entity.name.c_str(), XorStr("Valet_BasicC").c_str()))))
				{
					if (ItemDist < bLootRendering && CheckInScreen(entity.CurrentActor, Globals::Width, Globals::Height)) {
						Vector3 VehiclePosition = g_functions::ConvertWorld2Screen(ItemPosition);
						std::string Text = null + ("Vehicle [") + std::to_string((int)ItemDist) + ("m]");
						DrawString(14, VehiclePosition.x, VehiclePosition.y, &Col.red, true, true, Text.c_str());
					}
				}
				else if (strstr(entity.name.c_str(), ("AthenaSupplyDrop_C")) && g_loot && CheckInScreen(entity.CurrentActor, Globals::Width, Globals::Height))
				{
					if (ItemDist < bLootRendering) {
						Vector3 ChestPosition;
						ChestPosition = g_functions::ConvertWorld2Screen(ItemPosition);

						std::string Text = null + ("Supply Drop [") + std::to_string((int)ItemDist) + ("m]");
						DrawString(14, ChestPosition.x, ChestPosition.y, &Col.blue, true, true, Text.c_str());

					}
				}
				else if (strstr(entity.name.c_str(), ("Tiered_Ammo")) && g_ammo && CheckInScreen(entity.CurrentActor, Globals::Width, Globals::Height))
				{
					if (ItemDist < bLootRendering) {
						Vector3 ChestPosition;
						ChestPosition = g_functions::ConvertWorld2Screen(ItemPosition);
						std::string Text = null + ("Ammo Box [") + std::to_string((int)ItemDist) + ("m]");
						DrawString(14, ChestPosition.x, ChestPosition.y, &Col.white, true, true, Text.c_str());

					}
				}

				else if (g_loot && strstr(entity.name.c_str(), ("FortPickupAthena")) && CheckInScreen(entity.CurrentActor, Globals::Width, Globals::Height))
				{
					BYTE tier;
					if (ItemDist < bLootRendering) {

						auto definition = read<uint64_t>(g_pid, entity.CurrentActor + 0x308 + 0x18);

						tier = read<BYTE>(g_pid, definition + 0x74);

						RGBA Color;
						RGBA RGBAColor;

						Vector3 ChestPosition;
						ChestPosition = g_functions::ConvertWorld2Screen(ItemPosition);

						if (g_loot)
						{
							auto DisplayName = read<uint64_t>(g_pid, definition + 0x90); // DisplayName (FText)
							auto WeaponLength = read<uint32_t>(g_pid, DisplayName + 0x38); // FText -> Length
							wchar_t* WeaponName = new wchar_t[uint64_t(WeaponLength) + 1];

							Drive.ReadPtr(g_pid, (ULONG64)read<PVOID>(g_pid, DisplayName + 0x30), WeaponName, WeaponLength * sizeof(wchar_t));

							std::string Text = wchar_to_char(WeaponName);
							std::string wtf2 = Text + " [" + std::to_string((int)ItemDist) + ("m]");
							if (tier == 2)
							{
								Color = Col.green;
							}
							else if ((tier == 3))
							{
								Color = Col.blue;
							}
							else if ((tier == 4))
							{
								Color = Col.purple;
							}
							else if ((tier == 5))
							{
								Color = Col.yellow;
							}
							else if ((tier == 6))
							{
								Color = Col.yellow;
							}
							else if ((tier == 0) || (tier == 1))
							{
								Color = Col.white;
							}

							DrawString(14, ChestPosition.x, ChestPosition.y, &Color, true, true, wtf2.c_str());
						}
					}
				}
			}
		}

		for (Playertest p : PLIST)
		{
			auto identify = g_functions::f_getbonewithIndex(0, 0);
			g_functions::ConvertWorld2Screen(identify);

			uintptr_t RootComponent = read<uintptr_t>(g_pid, p.Acotr + 0x188);
			Globals::LocalPlayerRelativeLocation = read<Vector3>(g_pid, p.rootcomp + 0x128);

			if (Globals::LocalPawn)
			{
				const auto SetupCameraRotationAndFov = [](uintptr_t LocalPlayer, uintptr_t RootComponent, Vector3& CameraRotation, float& FOVAngle)
				{
					auto CameraRotChain_tmp0 = read<uint64_t>(g_pid, LocalPlayer + 0xC8);
					auto CameraRotChain_tmp1 = read<uint64_t>(g_pid, CameraRotChain_tmp0 + 0x8);

					camera::m_CameraRotation.x = read<double>(g_pid, CameraRotChain_tmp1 + 0xAE0);

					double tester = asin(camera::m_CameraRotation.x);
					double degreees = tester * (180.0 / M_PI);

					camera::m_CameraRotation.y = read<double>(g_pid, RootComponent + 0x148);


					camera::m_CameraRotation.x = degreees;

					//set fov angle
					if (g_fovchanger)
					{
						FOVAngle = FOVChangerValue;
					}
					else
					{
						FOVAngle = 80.0f / (read<double>(g_pid, CameraRotChain_tmp1 + 0x610) / 1.19f);
					}

				};
				SetupCameraRotationAndFov(Globals::LocalPlayer, Globals::LocalPawnRootComponent, camera::m_CameraRotation, camera::m_FovAngle);
			}
			else {
				// PlayerCameraManager -> LastFrameCameraCachePrivate -> POV -> Rotation && FOV
				camera::m_CameraRotation = read<Vector3>(g_pid, PlayerCameraManager + 0x28E0 + 0x10 + 0x18);
				camera::m_CameraRotation.z = 0;

				if (g_fovchanger)
				{
					camera::m_FovAngle = FOVChangerValue;
				}
				else
				{
					camera::m_FovAngle = read<float>(g_pid, PlayerCameraManager + 0x28E0 + 0x10 + 0x30); //0x2180
				}
			}

			// get camera location
			const auto SetupCameraLocation = [](uintptr_t LocalPlayer, Vector3& CameraLocation)
			{
				auto CameraLocChain_tmp0 = read<uint64_t>(g_pid, LocalPlayer + 0x70);
				auto CameraLocChain_tmp1 = read<uint64_t>(g_pid, CameraLocChain_tmp0 + 0x98);
				auto CameraLocChain_tmp2 = read<uint64_t>(g_pid, CameraLocChain_tmp1 + 0x180);

				CameraLocation = read<Vector3>(g_pid, CameraLocChain_tmp2 + 0x20);

			};
			SetupCameraLocation(Globals::LocalPlayer, camera::m_CameraLocation);


			uint64_t playerstate = read<uint64_t>(g_pid, p.Acotr + 0x2A0); 

			int TeamIndex = read<int>(g_pid, playerstate + 0x1020);

			if (g_fovchanger)
			{
				auto fov = PlayerCameraManager + 0x298;write(g_pid, fov, FOVChangerValue);
			}

			Vector3 vHeadBone = g_functions::f_getbonewithIndex(p.Acotrmesh, 98);
			Vector3 vRootBone = g_functions::f_getbonewithIndex(p.Acotrmesh, 0);

			Vector3 vHeadBoneOut = g_functions::ConvertWorld2Screen(Vector3(vHeadBone.x, vHeadBone.y, vHeadBone.z + 15));
			Vector3 vRootBoneOut = g_functions::ConvertWorld2Screen(vRootBone);

			Vector3 w2shead = g_functions::ConvertWorld2Screen(vHeadBone);

			Vector3 RootPos = g_functions::f_getbonewithIndex(p.Acotrmesh, 68);
			Vector3 RootPosOut = g_functions::ConvertWorld2Screen(RootPos);

			float BoxHeight = abs(vHeadBoneOut.y - vRootBoneOut.y);
			float BoxWidth = BoxHeight * 0.80;

			Vector3 RelativeInternalLocation = read<Vector3>(g_pid, RootComponent + 0x128);
			if (!RelativeInternalLocation.x && !RelativeInternalLocation.y) {
				continue;
			}

			Vector3 RelativeScreenLocation = g_functions::ConvertWorld2Screen(RelativeInternalLocation);
			if (!RelativeScreenLocation.x && !RelativeScreenLocation.y) {
				continue;
			}

			distance = Globals::LocalPlayerRelativeLocation.Distance(RelativeInternalLocation) / 100.f;

			if (g_playerfly)
			{
				write<float>(g_pid, Globals::LocalPawn + 0x1ce4, 10.f);
				write<bool>(g_pid, Globals::LocalPawn + 0x1c00 + 0x18, true);
			}

			if (g_gun_tracers)
			{
				auto CurrentWeapon = read<uintptr_t>(g_pid, p.Acotr + 0x7b0);
				write<bool>(g_pid, CurrentWeapon + 0xaf0, false);
				write<bool>(g_pid, CurrentWeapon + 0xaf1, false);
				write<bool>(g_pid, CurrentWeapon + 0xb00, false);
				write<bool>(g_pid, CurrentWeapon + 0xb02, false);
			}

			if (g_disable_gunshots)
			{
				auto CurrentWeapon = read<uintptr_t>(g_pid, p.Acotr + 0x7b0);
				write<bool>(g_pid, CurrentWeapon + 0x5a1, true);
				write<float>(g_pid, CurrentWeapon + 0xb04, 0.003);
				write<float>(g_pid, CurrentWeapon + 0x410, false);
				write<float>(g_pid, CurrentWeapon + 0x630, true);
				write<bool>(g_pid, CurrentWeapon + 0xa90, false);
				write<bool>(g_pid, CurrentWeapon + 0xa91, false);
			}

			if (g_name_esp)
			{
				//APlayerState	PlayerNamePrivate	0x370	FString
				uint64_t APlayerState = read<uint64_t>(g_pid, Globals::LocalPawn + 0x0);
				auto nameptr = read<uintptr_t>(g_pid, APlayerState + 0x370);

				uint64_t StringData = read<uint64_t>(g_pid, nameptr); //FString -> Data (0x0)
				uint32_t StringLength = read<uint32_t>(g_pid, nameptr + 0x8); //FString -> Count (0x8)
				wchar_t* OutString = new wchar_t[StringLength + 1];
				Drive.ReadPtr(g_pid, StringData, OutString, StringLength * sizeof(wchar_t));

				std::string Text = wchar_to_char(OutString);

				DrawString(13, vHeadBoneOut.x, vHeadBoneOut.y - 50, &Col.white, true, true, Text.c_str());
			}

			if (g_platform_esp && CheckInScreen(p.Acotr, Globals::Width, Globals::Height))
			{
				//AFortPlayerState	Platform	0x420	FString
				uint64_t AFortPlayerState = read<uint64_t>(g_pid, Globals::LocalPawn + 0x0);
				auto nameptr = read<uintptr_t>(g_pid, AFortPlayerState + 0x420);

				uint64_t StringData = read<uint64_t>(g_pid, nameptr); //FString -> Data (0x0)
				uint32_t StringLength = read<uint32_t>(g_pid, nameptr + 0x8); //FString -> Count (0x8)
				wchar_t* OutString = new wchar_t[StringLength + 1];
				Drive.ReadPtr(g_pid, StringData, OutString, StringLength * sizeof(wchar_t));


				std::string Text = wchar_to_char(OutString);

				//std::cout << namehaha << "\n";
				
				if (strstr(Text.c_str(), ("WIN")))
				{
					DrawString(13, vHeadBoneOut.x, vHeadBoneOut.y - 20, &Col.white, true, true, "PC");

				}
				else if (strstr(Text.c_str(), ("XBL")) || strstr(Text.c_str(), ("XSX")))
				{
					DrawString(13, vHeadBoneOut.x, vHeadBoneOut.y - 20, &Col.green, true, true, "Xbox");

				}
				else if (strstr(Text.c_str(), ("PSN")) || strstr(Text.c_str(), ("PS5")))
				{
					DrawString(13, vHeadBoneOut.x, vHeadBoneOut.y - 20, &Col.lightblue, true, true, "PSN");

				}
				else if (strstr(Text.c_str(), ("SWT")))
				{
					DrawString(13, vHeadBoneOut.x, vHeadBoneOut.y - 20, &Col.red, true, true, "Nintendo");

				}
				else
				{
					DrawString(13, vHeadBoneOut.x, vHeadBoneOut.y - 20, &Col.white, true, true, Text.c_str());
				}

			}

			Vector3 neck2 = g_functions::f_getbonewithIndex(p.Acotrmesh, 98);
			Vector3 vneck2 = g_functions::ConvertWorld2Screen(neck2);
			Vector3 pelvis = g_functions::f_getbonewithIndex(p.Acotrmesh, 2);
			Vector3 vpelvis = g_functions::ConvertWorld2Screen(pelvis);
			Vector3 bottom1 = g_functions::ConvertWorld2Screen(Vector3(vHeadBone.x + 10, vHeadBone.y - 10, vHeadBone.z));
			Vector3 bottom2 = g_functions::ConvertWorld2Screen(Vector3(vHeadBone.x - 10, vHeadBone.y - 10, vHeadBone.z));
			Vector3 bottom3 = g_functions::ConvertWorld2Screen(Vector3(vHeadBone.x - 10, vHeadBone.y + 10, vHeadBone.z));
			Vector3 bottom4 = g_functions::ConvertWorld2Screen(Vector3(vHeadBone.x + 10, vHeadBone.y + 10, vHeadBone.z));

			Vector3 top1 = g_functions::ConvertWorld2Screen(Vector3(vHeadBone.x + 10, vHeadBone.y - 10, vHeadBone.z + 15));
			Vector3 top2 = g_functions::ConvertWorld2Screen(Vector3(vHeadBone.x - 10, vHeadBone.y - 10, vHeadBone.z + 15));
			Vector3 top3 = g_functions::ConvertWorld2Screen(Vector3(vHeadBone.x - 10, vHeadBone.y + 10, vHeadBone.z + 15));
			Vector3 top4 = g_functions::ConvertWorld2Screen(Vector3(vHeadBone.x + 10, vHeadBone.y + 10, vHeadBone.z + 15));

			ImU32 Cay = ImGui::GetColorU32({ 255, 215, 0, 255 });

			ImGui::GetForegroundDrawList()->AddLine(ImVec2(bottom1.x, bottom1.y), ImVec2(top1.x, top1.y), Cay, 0.1f);
			ImGui::GetForegroundDrawList()->AddLine(ImVec2(bottom2.x, bottom2.y), ImVec2(top2.x, top2.y), Cay, 0.1f);
			ImGui::GetForegroundDrawList()->AddLine(ImVec2(bottom3.x, bottom3.y), ImVec2(top3.x, top3.y), Cay, 0.1f);
			ImGui::GetForegroundDrawList()->AddLine(ImVec2(bottom4.x, bottom4.y), ImVec2(top4.x, top4.y), Cay, 0.1f);

			ImGui::GetForegroundDrawList()->AddLine(ImVec2(bottom1.x, bottom1.y), ImVec2(bottom2.x, bottom2.y), Cay, 0.1f);
			ImGui::GetForegroundDrawList()->AddLine(ImVec2(bottom2.x, bottom2.y), ImVec2(bottom3.x, bottom3.y), Cay, 0.1f);
			ImGui::GetForegroundDrawList()->AddLine(ImVec2(bottom3.x, bottom3.y), ImVec2(bottom4.x, bottom4.y), Cay, 0.1f);
			ImGui::GetForegroundDrawList()->AddLine(ImVec2(bottom4.x, bottom4.y), ImVec2(bottom1.x, bottom1.y), Cay, 0.1f);

			ImGui::GetForegroundDrawList()->AddLine(ImVec2(top1.x, top1.y), ImVec2(top2.x, top2.y), Cay, 0.1f);
			ImGui::GetForegroundDrawList()->AddLine(ImVec2(top2.x, top2.y), ImVec2(top3.x, top3.y), Cay, 0.1f);
			ImGui::GetForegroundDrawList()->AddLine(ImVec2(top3.x, top3.y), ImVec2(top4.x, top4.y), Cay, 0.1f);
			ImGui::GetForegroundDrawList()->AddLine(ImVec2(top4.x, top4.y), ImVec2(top1.x, top1.y), Cay, 0.1f);
			RGBA Skellll;
			Skellll = { 255, 255, 255, 255 };
			DrawLine(vneck2.x, vneck2.y, vpelvis.x, vpelvis.y, &Skellll, 0.5f);

			if (g_curweaponesp && CheckInScreen(p.Acotr, Globals::Width, Globals::Height))
			{
				uintptr_t ItemRootComponent = read<uintptr_t>(g_pid, p.Acotr + 0x188);
				Vector3 ItemPosition = read<Vector3>(g_pid, ItemRootComponent + 0x128);
				float ItemDist = Globals::LocalPlayerRelativeLocation.Distance(ItemPosition) / 100.f;

				if (ItemDist < bLootRendering) {

					auto CurrentWeapon = read<uintptr_t>(g_pid, p.Acotr + 0x7A0);//AFortPawn	CurrentWeapon	0x7b0	AFortWeapon*
					auto ItemData = read<DWORD_PTR>(g_pid, CurrentWeapon + 0x3E8);

					BYTE tier;
					tier = read<BYTE>(g_pid, ItemData + 0x74);
					ImColor Color;
					if (tier == 2)
					{
						Color = IM_COL32(0, 255, 0, 255);
					}
					else if ((tier == 3))
					{
						Color = IM_COL32(0, 0, 255, 255);
					}
					else if ((tier == 4))
					{
						Color = IM_COL32(128, 0, 128, 255); // p
					}
					else if ((tier == 5))
					{
						Color = IM_COL32(255, 255, 0, 255); // p
					}
					else if ((tier == 6))
					{
						Color = IM_COL32(255, 255, 0, 255); // p
					}
					else if ((tier == 0) || (tier == 1))
					{
						Color = IM_COL32(255, 255, 255, 255); // p
					}

					auto AmmoCount = read<int>(g_pid, CurrentWeapon + 0xB24);//AFortWeapon	AmmoCount	0xb3c	int32_t


					auto bIsReloadingWeapon = read<bool>(g_pid, CurrentWeapon + 0x321);

					auto DisplayName = read<uint64_t>(g_pid, ItemData + 0x90); // DisplayName (FText)
					auto WeaponLength = read<uint32_t>(g_pid, DisplayName + 0x38); // FText -> Length
					wchar_t* WeaponName = new wchar_t[uint64_t(WeaponLength) + 1];

					Drive.ReadPtr(g_pid, (ULONG64)read<PVOID>(g_pid, DisplayName + 0x30), WeaponName, WeaponLength * sizeof(wchar_t));

					std::string Text = wchar_to_char(WeaponName);


					if (strstr(p.name.c_str(), "PlayerPawn_Athena_C"))
					{
						std::string Player = XorStr("Player").c_str();
						ImVec2 TextSize = ImGui::CalcTextSize(Player.c_str());
						ImGui::GetOverlayDrawList()->AddText(ImVec2(vHeadBoneOut.x - 30 - TextSize.x / 2, vHeadBoneOut.y - 15 - TextSize.y / 2), ImGui::GetColorU32({ 255, 255, 255, 255 }), Player.c_str());
					}
					else
					{
						std::string Bot = XorStr("Bot/Npc").c_str();
						ImVec2 TextSize = ImGui::CalcTextSize(Bot.c_str());
						ImGui::GetOverlayDrawList()->AddText(ImVec2(vHeadBoneOut.x - 30 - TextSize.x / 2, vHeadBoneOut.y - 15 - TextSize.y / 2), ImGui::GetColorU32({ 255, 255, 255, 255 }), Bot.c_str());
					}

						if (AmmoCount)
						{
							char buffer[128];
							sprintf_s(buffer, "Ammo: %i", AmmoCount);
							if (buffer != "?") 
							{
								ImGui::GetOverlayDrawList()->AddText(ImVec2(vpelvis.x, vpelvis.y + 15), IM_COL32(255, 255, 255, 255), buffer);
							}
						}

						if (bIsReloadingWeapon)
						{
							ImGui::GetOverlayDrawList()->AddText(ImVec2(vpelvis.x - 30, vpelvis.y), IM_COL32(255, 255, 255, 255), "Reloading");
						}
						else
						{
								ImGui::GetOverlayDrawList()->AddText(ImVec2(vpelvis.x - 30, vpelvis.y), Color, Text.c_str());
						}

				}
			}
			int Teamcheck;

			if (g_chams)
			{
				Teamcheck = TeamIndex = LocalTeam;
			}
			else
			{
				Teamcheck = TeamIndex != LocalTeam; 
			}


			//auto isDying = (read<char>(g_pid, p.Acotr + 0x6a0) >> 2) & 1;

			if (Teamcheck || InLobby) {

				isVis = isVisible(p.Acotrmesh);
				if (distance <= bE5pD1st4nce || InLobby) {
					if (g_boxesp && CheckInScreen(p.Acotr, Globals::Width, Globals::Height))
					{
						if (isVis)
						{
							DrawNormalBox(vRootBoneOut.x - (BoxWidth / 2), vHeadBoneOut.y, BoxWidth, BoxHeight, 1.0f, &ESPColor2, &ESPColor2);
						}
						else
						{
							DrawNormalBox(vRootBoneOut.x - (BoxWidth / 2), vHeadBoneOut.y, BoxWidth, BoxHeight, 1.0f, &ESPColor, &ESPColor);
						}
					}
					else if (g_cornerboxesp && CheckInScreen(p.Acotr, Globals::Width, Globals::Height))
					{
						if (isVis)
						{
							DrawCorneredBox(vRootBoneOut.x - (BoxWidth / 2), vHeadBoneOut.y, BoxWidth, BoxHeight, IM_COL32(255, 215, 0, 255), 1.5);
						}
						else
						{
							DrawCorneredBox(vRootBoneOut.x - (BoxWidth / 2), vHeadBoneOut.y, BoxWidth, BoxHeight, IM_COL32(255, 255, 255, 255), 1.5);
						}
					}
					if (g_esp_distance && CheckInScreen(p.Acotr, Globals::Width, Globals::Height)) {

						char dist[64];
						sprintf_s(dist, "%.fM", distance);

						ImVec2 TextSize = ImGui::CalcTextSize(dist);
						ImGui::GetOverlayDrawList()->AddText(ImVec2(vRootBoneOut.x - 15 - TextSize.x / 2, vRootBoneOut.y - 15 - TextSize.y / 2), ImGui::GetColorU32({ 255, 255, 255, 255 }), dist);
					}
					if (g_esp_skeleton && CheckInScreen(p.Acotr, Globals::Width, Globals::Height)) {

						Vector3 neck2 = g_functions::f_getbonewithIndex(p.Acotrmesh, 98);
						Vector3 vneck2 = g_functions::ConvertWorld2Screen(neck2);

						Vector3 neck = g_functions::f_getbonewithIndex(p.Acotrmesh, 66);
						Vector3 vneck = g_functions::ConvertWorld2Screen(neck);
						Vector3 rightChest = g_functions::f_getbonewithIndex(p.Acotrmesh, 8);
						Vector3 vrightChest = g_functions::ConvertWorld2Screen(rightChest);
						Vector3 leftChest = g_functions::f_getbonewithIndex(p.Acotrmesh, 37);
						Vector3 vleftChest = g_functions::ConvertWorld2Screen(leftChest);
						Vector3 leftShoulder = g_functions::f_getbonewithIndex(p.Acotrmesh, 38);
						Vector3 vleftShoulder = g_functions::ConvertWorld2Screen(leftShoulder);
						Vector3 rightShoulder = g_functions::f_getbonewithIndex(p.Acotrmesh, 9);
						Vector3 vrightShoulder = g_functions::ConvertWorld2Screen(rightShoulder);
						Vector3 leftElbow = g_functions::f_getbonewithIndex(p.Acotrmesh, 94);
						Vector3 vleftElbow = g_functions::ConvertWorld2Screen(leftElbow);
						Vector3 rightElbow = g_functions::f_getbonewithIndex(p.Acotrmesh, 10);
						Vector3 vrightElbow = g_functions::ConvertWorld2Screen(rightElbow);
						Vector3 leftWrist = g_functions::f_getbonewithIndex(p.Acotrmesh, 62);
						Vector3 vleftWrist = g_functions::ConvertWorld2Screen(leftWrist);
						Vector3 rightWrist = g_functions::f_getbonewithIndex(p.Acotrmesh, 33);
						Vector3 vrightWrist = g_functions::ConvertWorld2Screen(rightWrist);
						Vector3 pelvis = g_functions::f_getbonewithIndex(p.Acotrmesh, 2);
						Vector3 vpelvis = g_functions::ConvertWorld2Screen(pelvis);
						Vector3 leftAss = g_functions::f_getbonewithIndex(p.Acotrmesh, 76);
						Vector3 vleftAss = g_functions::ConvertWorld2Screen(leftAss);
						Vector3 rightAss = g_functions::f_getbonewithIndex(p.Acotrmesh, 69);
						Vector3 vrightAss = g_functions::ConvertWorld2Screen(rightAss);
						Vector3 leftKnee = g_functions::f_getbonewithIndex(p.Acotrmesh, 77);
						Vector3 vleftKnee = g_functions::ConvertWorld2Screen(leftKnee);
						Vector3 rightKnee = g_functions::f_getbonewithIndex(p.Acotrmesh, 70);
						Vector3 vrightKnee = g_functions::ConvertWorld2Screen(rightKnee);
						Vector3 leftAnkle = g_functions::f_getbonewithIndex(p.Acotrmesh, 78);
						Vector3 vleftAnkle = g_functions::ConvertWorld2Screen(leftAnkle);
						Vector3 rightAnkle = g_functions::f_getbonewithIndex(p.Acotrmesh, 71);
						Vector3 vrightAnkle = g_functions::ConvertWorld2Screen(rightAnkle);

						RGBA ESPSkeleton;

						if (isVis)
						{
							ESPSkeleton = { 255, 215, 0, 255 };
						}
						else
						{
							ESPSkeleton = { 255, 255, 255, 255 };
						}

						DrawLine(vleftChest.x, vleftChest.y, vrightChest.x, vrightChest.y, &ESPSkeleton, 0.5f);
						DrawLine(vleftChest.x, vleftChest.y, vleftShoulder.x, vleftShoulder.y, &ESPSkeleton, 0.5f);
						DrawLine(vrightChest.x, vrightChest.y, vrightShoulder.x, vrightShoulder.y, &ESPSkeleton, 0.5f);
						DrawLine(vleftShoulder.x, vleftShoulder.y, vleftElbow.x, vleftElbow.y, &ESPSkeleton, 0.5f);
						DrawLine(vrightShoulder.x, vrightShoulder.y, vrightElbow.x, vrightElbow.y, &ESPSkeleton, 0.5f);
						DrawLine(vleftElbow.x, vleftElbow.y, vleftWrist.x, vleftWrist.y, &ESPSkeleton, 0.5f);
						DrawLine(vrightElbow.x, vrightElbow.y, vrightWrist.x, vrightWrist.y, &ESPSkeleton, 0.5f);
						DrawLine(vneck2.x, vneck2.y, vpelvis.x, vpelvis.y, &ESPSkeleton, 0.5f);
						DrawLine(vpelvis.x, vpelvis.y, vleftAss.x, vleftAss.y, &ESPSkeleton, 0.5f);
						DrawLine(vpelvis.x, vpelvis.y, vrightAss.x, vrightAss.y, &ESPSkeleton, 0.5f);
						DrawLine(vleftAss.x, vleftAss.y, vleftKnee.x, vleftKnee.y, &ESPSkeleton, 0.5f);
						DrawLine(vrightAss.x, vrightAss.y, vrightKnee.x, vrightKnee.y, &ESPSkeleton, 0.5f);
						DrawLine(vleftKnee.x, vleftKnee.y, vleftAnkle.x, vleftAnkle.y, &ESPSkeleton, 0.5f);
						DrawLine(vrightKnee.x, vrightKnee.y, vrightAnkle.x, vrightAnkle.y, &ESPSkeleton, 0.5f);
					}

					if (g_3d_box && CheckInScreen(p.Acotr, Globals::Width, Globals::Height))
					{

						if (vHeadBoneOut.x != 0 || vHeadBoneOut.y != 0 || vHeadBoneOut.z != 0)
						{

							ImU32 ESPSkeleton;

							if (isVis)
							{
								ESPSkeleton = ImGui::GetColorU32({ 255, 215, 0, 255 });
							}
							else
							{
								ESPSkeleton = ImGui::GetColorU32({ 255, 255, 255, 255 });
							}

							Vector3 bottom1 = g_functions::ConvertWorld2Screen(Vector3(vRootBone.x + 40, vRootBone.y - 40, vRootBone.z));
							Vector3 bottom2 = g_functions::ConvertWorld2Screen(Vector3(vRootBone.x - 40, vRootBone.y - 40, vRootBone.z));
							Vector3 bottom3 = g_functions::ConvertWorld2Screen(Vector3(vRootBone.x - 40, vRootBone.y + 40, vRootBone.z));
							Vector3 bottom4 = g_functions::ConvertWorld2Screen(Vector3(vRootBone.x + 40, vRootBone.y + 40, vRootBone.z));

							Vector3 top1 = g_functions::ConvertWorld2Screen(Vector3(vHeadBone.x + 40, vHeadBone.y - 40, vHeadBone.z + 15));
							Vector3 top2 = g_functions::ConvertWorld2Screen(Vector3(vHeadBone.x - 40, vHeadBone.y - 40, vHeadBone.z + 15));
							Vector3 top3 = g_functions::ConvertWorld2Screen(Vector3(vHeadBone.x - 40, vHeadBone.y + 40, vHeadBone.z + 15));
							Vector3 top4 = g_functions::ConvertWorld2Screen(Vector3(vHeadBone.x + 40, vHeadBone.y + 40, vHeadBone.z + 15));

							ImGui::GetForegroundDrawList()->AddLine(ImVec2(bottom1.x, bottom1.y), ImVec2(top1.x, top1.y), ESPSkeleton, 0.1f);
							ImGui::GetForegroundDrawList()->AddLine(ImVec2(bottom2.x, bottom2.y), ImVec2(top2.x, top2.y), ESPSkeleton, 0.1f);
							ImGui::GetForegroundDrawList()->AddLine(ImVec2(bottom3.x, bottom3.y), ImVec2(top3.x, top3.y), ESPSkeleton, 0.1f);
							ImGui::GetForegroundDrawList()->AddLine(ImVec2(bottom4.x, bottom4.y), ImVec2(top4.x, top4.y), ESPSkeleton, 0.1f);

							ImGui::GetForegroundDrawList()->AddLine(ImVec2(bottom1.x, bottom1.y), ImVec2(bottom2.x, bottom2.y), ESPSkeleton, 0.1f);
							ImGui::GetForegroundDrawList()->AddLine(ImVec2(bottom2.x, bottom2.y), ImVec2(bottom3.x, bottom3.y), ESPSkeleton, 0.1f);
							ImGui::GetForegroundDrawList()->AddLine(ImVec2(bottom3.x, bottom3.y), ImVec2(bottom4.x, bottom4.y), ESPSkeleton, 0.1f);
							ImGui::GetForegroundDrawList()->AddLine(ImVec2(bottom4.x, bottom4.y), ImVec2(bottom1.x, bottom1.y), ESPSkeleton, 0.1f);

							ImGui::GetForegroundDrawList()->AddLine(ImVec2(top1.x, top1.y), ImVec2(top2.x, top2.y), ESPSkeleton, 0.1f);
							ImGui::GetForegroundDrawList()->AddLine(ImVec2(top2.x, top2.y), ImVec2(top3.x, top3.y), ESPSkeleton, 0.1f);
							ImGui::GetForegroundDrawList()->AddLine(ImVec2(top3.x, top3.y), ImVec2(top4.x, top4.y), ESPSkeleton, 0.1f);
							ImGui::GetForegroundDrawList()->AddLine(ImVec2(top4.x, top4.y), ImVec2(top1.x, top1.y), ESPSkeleton, 0.1f);
						}
					}

					if (g_lineesp && CheckInScreen(p.Acotr, Globals::Width, Globals::Height))
					{

						ImU32 LineColor;

						if (isVis)
						{
							LineColor = ImGui::ColorConvertFloat4ToU32(ImVec4(ESPColor2.R / 128.0, ESPColor2.G / 224.0, ESPColor2.B / 0.0, ESPColor2.A / 255.0));
						}
						else
						{
							LineColor = ImGui::ColorConvertFloat4ToU32(ImVec4(ESPColor.R / 128.0, ESPColor.G / 224.0, ESPColor.B / 0.0, ESPColor.A / 255.0));
						}

						ImGui::GetOverlayDrawList()->AddLine(
							ImVec2(Globals::ScreenCenterX, Globals::Height),
							ImVec2(RelativeScreenLocation.x, RelativeScreenLocation.y + 30),
							LineColor,
							0.1f
						);
					}
				}
				

				auto dx = w2shead.x - (Globals::Width / 2);
				auto dy = w2shead.y - (Globals::Height / 2);
				auto dz = w2shead.z - (Depth / 2);

				auto dist = sqrtf(dx * dx + dy * dy + dz * dz) / 100.0f;

				auto isDBNO = (read<char>(g_pid, p.Acotr + 0x702) >> 4) & 1;



				if (dist < bA1mb0tF0VV4lue && dist < closestDistance && TeamIndex != LocalTeam && !InLobby && CheckInScreen(p.Acotr, Globals::Width, Globals::Height))
				{
					if (g_skipknocked)
					{
						if (!isDBNO)
						{
							closestDistance = dist;
							closestPawn = p.Acotr;
						}
					}
					else
					{
						closestDistance = dist;
						closestPawn = p.Acotr;
					}

				}
			}
		}
		if (closestPawn != 0)
		{
			if (closestPawn && Key.IsKeyPushing(hotkeys::aimkey) or Controller::IsPressingLeftTrigger() && isFortniteFocused)
			{
				if (g_aimbot)
				{
					targetlocked = true;

					RGBA ESPTargetLine = { 255, 0, 0, 255 };

					uint64_t AimbotMesh = read<uint64_t>(g_pid, closestPawn + 0x300);
					if (!AimbotMesh)
						return false;

					Vector3 HeadPosition = g_functions::f_getbonewithIndex(AimbotMesh, select_hitbox());
					if (!IsVec3Valid(HeadPosition))
						return false;

					Vector3 Head = g_functions::ConvertWorld2Screen(HeadPosition);
					if (!IsVec3Valid(HeadPosition))
						return false;

					if (Head.x != 0 || Head.y != 0 || Head.z != 0)
					{
						if ((GetDistance(Head.x, Head.y, Head.z, Globals::Width / 2, Globals::Height / 2) <= bA1mb0tF0VV4lue))
						{
							if (g_mouse_aim)
							{
								aimbot(Head.x, Head.y);
							}
							else if (g_mem_aim)
							{
								Vector3 HeadPosition = g_functions::f_getbonewithIndex(AimbotMesh, 78);

								if (!IsVec3Valid(HeadPosition))
									return false;

								WriteAngles(HeadPosition);
										
							}

							if (g_exploits_backtrack)
							{
								if (GetAsyncKeyState(VK_LBUTTON)) {

									float ItemDist = Globals::LocalPlayerRelativeLocation.Distance(Head) / 100.f;

									auto fov = sqrtf(powf(Head.x, 4.0f) + powf(Head.y, 4.0f));
									int setting = 2;

									//if (fov < setting && ItemDist < bA1mD1st4nce) {
									write<float>(g_pid, closestPawn + 0x64, 0.001f); // time dilation 0x98
								//}

								}
								else
								{
									write<float>(g_pid, closestPawn + 0x64, 1); // time dilation 0x98
								}
							}
						}

					}
				}
			}
			else
			{
				isaimbotting = false;
				targetlocked = false;

				closestDistance = FLT_MAX;
				closestPawn = NULL;
			}
		}


	}

	catch (...) {}
}
#include <array>

std::uintptr_t find_signature(const char* sig, const char* mask)
{
	auto buffer = std::make_unique<std::array<std::uint8_t, 0x100000>>();
	auto data = buffer.get()->data();

	for (std::uintptr_t i = 0u; i < (2u << 25u); ++i)
	{
		Drive.ReadPtr(g_pid, g_base_address + i * 0x100000, data, 0x100000);

		if (!data)
			return 0;

		for (std::uintptr_t j = 0; j < 0x100000u; ++j)
		{
			if ([](std::uint8_t const* data, std::uint8_t const* sig, char const* mask)
				{
					for (; *mask; ++mask, ++data, ++sig)
					{
						if (*mask == 'x' && *data != *sig) return false;
					}
					return (*mask) == 0;
				}(data + j, (std::uint8_t*)sig, mask))
			{
				std::uintptr_t result = g_base_address + i * 0x100000 + j;
				std::uint32_t rel = 0;

				Drive.ReadPtr(g_pid, result + 3, &rel, sizeof(std::uint32_t));

				if (!rel)
					return 0;

				return result + rel + 7;
			}
		}
	}

	return 0;
}

void background()
{
	ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize, ImGuiCond_Once);

	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.09f, 0.09f, 0.09f, 0.20f / 1.f * 2.f));
	static const auto flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoMove;
	ImGui::Begin(XorStr("##background").c_str(), nullptr, flags);
	ImGui::End();
	ImGui::PopStyleColor();
}

void cursor()
{
	ImGui::SetNextWindowPos(ImGui::GetIO().MousePos);
	ImGui::SetNextWindowSize(ImVec2(0, 0), ImGuiCond_Once);


	//ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.09f, 0.09f, 0.09f, 0.20f / 1.f * 2.f));
	static const auto flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoMove;
	ImGui::Begin(XorStr("##cursor").c_str(), nullptr, flags);
	ImGui::End();
	//ImGui::PopStyleColor();
}

ImColor cRainbow = ImGui::ColorConvertFloat4ToU32(ImVec4(255.0, 0.0, 0.0, 255.0));


void decoration()
{
	auto pos = ImGui::GetWindowPos();
	auto size = ImGui::GetWindowSize();
	ImGui::GetWindowDrawList()->AddRect(ImVec2(pos.x + 5, pos.y + 1), ImVec2(pos.x + size.x - 5, pos.y + size.y - 1), ImColor(1, 1, 1, 255));
	ImGui::GetWindowDrawList()->AddRect(ImVec2(pos.x + 6, pos.y + 2), ImVec2(pos.x + size.x - 6, pos.y + size.y - 2), ImColor(40, 40, 40, 255));
	ImGui::GetWindowDrawList()->AddRect(ImVec2(pos.x + 7, pos.y + 3), ImVec2(pos.x + size.x - 7, pos.y + size.y - 3), ImColor(1, 1, 1, 255));
	ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(pos.x + 8, pos.y + 4), ImVec2(pos.x + size.x - 8, pos.y + 6), cRainbow);
	ImGui::GetWindowDrawList()->AddRect(ImVec2(pos.x + 7, pos.y + 5), ImVec2(pos.x + size.x - 7, pos.y + size.y - 3), ImColor(1, 1, 1, 255));
	ImGui::GetWindowDrawList()->AddRect(ImVec2(pos.x + 7, pos.y + 6), ImVec2(pos.x + size.x - 7, pos.y + size.y - 3), ImColor(40, 40, 40, 255));
	ImGui::GetWindowDrawList()->AddRect(ImVec2(pos.x + 7, pos.y + 7), ImVec2(pos.x + size.x - 7, pos.y + size.y - 3), ImColor(1, 1, 1, 255));
	ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(pos.x + 8, pos.y + 8), ImVec2(pos.x + size.x - 8, pos.y + size.y - 4), ImColor(30, 30, 30, 255));

	ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(pos.x + 12, pos.y + 23), ImVec2(pos.x + size.x - 12, pos.y + size.y - 8), ImColor(1, 1, 1, 255));
	ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(pos.x + 13, pos.y + 24), ImVec2(pos.x + size.x - 13, pos.y + size.y - 9), ImColor(40, 40, 40, 255));
	ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(pos.x + 17, pos.y + 28), ImVec2(pos.x + size.x - 17, pos.y + size.y - 13), ImColor(1, 1, 1, 255));
	ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(pos.x + 18, pos.y + 29), ImVec2(pos.x + size.x - 18, pos.y + size.y - 14), ImColor(34, 34, 34, 255));
	ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(pos.x + 18, pos.y + 29), ImVec2(pos.x + 133, pos.y + size.y - 14), ImColor(31, 31, 31, 255));


	ImGui::GetWindowDrawList()->AddText(ImVec2(pos.x + 14, pos.y + 8), ImColor(255, 255, 255, 255), "Gloomy");
	ImGui::GetWindowDrawList()->AddText(ImVec2(pos.x + 50, pos.y + 8), ImColor(255, 0, 0, 255), ".cc");
	ImGui::GetWindowDrawList()->AddRect(ImVec2(pos.x + 17, pos.y + 28), ImVec2(pos.x + 133, pos.y + size.y - 13), ImColor(1, 1, 1, 255));
}

bool Head = true, Neck, Chest;

void runRenderTick() {
	glfwPollEvents();

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	RECT rect = { 0 };

	HWND GameWnd = FindWindowA(XorStr("UnrealWindow").c_str(), XorStr("Fortnite  ").c_str());

	HWND hwnd_active = GetForegroundWindow();
	HWND overlay_window = glfwGetWin32Window(g_window);

	if (hwnd_active == GameWnd || hwnd_active == overlay_window) {
		isFortniteFocused = true;
	}
	else {
		isFortniteFocused = true;
	}

	if (isFortniteFocused)
	{
		if (GetWindowRect(GameWnd, &rect))
		{
			Globals::Width = rect.right - rect.left;
			Globals::Height = rect.bottom - rect.top;
		}

		Globals::ScreenCenterX = (Globals::Width / 2.0f);
		Globals::ScreenCenterY = (Globals::Height / 2.0f);

		actorLoop();
	}

	float radiusx = bA1mb0tF0VV4lue * (Globals::ScreenCenterX / 100);
	float radiusy = bA1mb0tF0VV4lue * (Globals::ScreenCenterY / 100);

	float calcradius = (radiusx + radiusy) / 2;

	if (Globals::LocalPawn)
	{
		if (g_fov)
		{
			DrawLine((float)(Globals::Width / 2) - bA1mb0tF0VV4lue, (float)(Globals::Height / 2) - bA1mb0tF0VV4lue, (float)(Globals::Width / 2) + bA1mb0tF0VV4lue, (float)(Globals::Height / 2) - bA1mb0tF0VV4lue, &Col.white, 2);
			DrawLine((float)(Globals::Width / 2) - bA1mb0tF0VV4lue, (float)(Globals::Height / 2) + bA1mb0tF0VV4lue, (float)(Globals::Width / 2) + bA1mb0tF0VV4lue, (float)(Globals::Height / 2) + bA1mb0tF0VV4lue, &Col.white, 2);
			DrawLine((float)(Globals::Width / 2) + bA1mb0tF0VV4lue, (float)(Globals::Height / 2) - bA1mb0tF0VV4lue, (float)(Globals::Width / 2) + bA1mb0tF0VV4lue, (float)(Globals::Height / 2) + bA1mb0tF0VV4lue, &Col.white, 2);
			DrawLine((float)(Globals::Width / 2) - bA1mb0tF0VV4lue, (float)(Globals::Height / 2) - bA1mb0tF0VV4lue, (float)(Globals::Width / 2) - bA1mb0tF0VV4lue, (float)(Globals::Height / 2) + bA1mb0tF0VV4lue, &Col.white, 2);
			DrawLine((float)(Globals::Width / 2) - bA1mb0tF0VV4lue, (float)(Globals::Height / 2) - bA1mb0tF0VV4lue, (float)(Globals::Width / 2) + bA1mb0tF0VV4lue, (float)(Globals::Height / 2) - bA1mb0tF0VV4lue, &Col.white, 3);
			DrawLine((float)(Globals::Width / 2) - bA1mb0tF0VV4lue, (float)(Globals::Height / 2) + bA1mb0tF0VV4lue, (float)(Globals::Width / 2) + bA1mb0tF0VV4lue, (float)(Globals::Height / 2) + bA1mb0tF0VV4lue, &Col.white, 3);
			DrawLine((float)(Globals::Width / 2) - bA1mb0tF0VV4lue, (float)(Globals::Height / 2) - bA1mb0tF0VV4lue, (float)(Globals::Width / 2) + bA1mb0tF0VV4lue, (float)(Globals::Height / 2) - bA1mb0tF0VV4lue, &Col.black_, 1);
			DrawLine((float)(Globals::Width / 2) - bA1mb0tF0VV4lue, (float)(Globals::Height / 2) + bA1mb0tF0VV4lue, (float)(Globals::Width / 2) + bA1mb0tF0VV4lue, (float)(Globals::Height / 2) + bA1mb0tF0VV4lue, &Col.black_, 1);
			DrawLine((float)(Globals::Width / 2) + bA1mb0tF0VV4lue, (float)(Globals::Height / 2) - bA1mb0tF0VV4lue, (float)(Globals::Width / 2) + bA1mb0tF0VV4lue, (float)(Globals::Height / 2) + bA1mb0tF0VV4lue, &Col.black_, 1);
			DrawLine((float)(Globals::Width / 2) - bA1mb0tF0VV4lue, (float)(Globals::Height / 2) - bA1mb0tF0VV4lue, (float)(Globals::Width / 2) - bA1mb0tF0VV4lue, (float)(Globals::Height / 2) + bA1mb0tF0VV4lue, &Col.black_, 1);
		}

		if (g_crossh) {
			DrawLine((float)(Globals::Width / 2), (float)(Globals::Height / 2) - 8, (float)(Globals::Width / 2), (float)(Globals::Height / 2) + 8, &Col.white, 1);
			DrawLine((float)(Globals::Width / 2) - 8, (float)(Globals::Height / 2), (float)(Globals::Width / 2) + 8, (float)(Globals::Height / 2), &Col.white, 1);
		}

		if (g_circlefov) {
			ImGui::GetOverlayDrawList()->AddCircle(ImVec2(Globals::ScreenCenterX, Globals::ScreenCenterY), bA1mb0tF0VV4lue, ImGui::ColorConvertFloat4ToU32(ImVec4(ESPColor.R / 255.0, ESPColor.G / 255.0, ESPColor.B / 255.0, ESPColor.A / 255.0)), 100);
		}
	}
	DrawText1(15, 15, "Gloomy", &Col.red);
	DrawText1(15, 28, "F1 - MENU", &Col.white);

	char dist[64];
	sprintf_s(dist, "FPS %.f\n", ImGui::GetIO().Framerate);
	ImGui::GetOverlayDrawList()->AddText(ImVec2(15, 40), ImColor(cRainbow), dist);

	if (g_overlay_visible)
	{
		cursor();
		background();

	}
	ImGuiIO& io = ImGui::GetIO();

	if (g_overlay_visible) {
		{

			ImGui::Begin(XorStr(" ").c_str(), nullptr,  ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar); //  | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground
			ImGui::SetWindowSize(XorStr(" ").c_str(), ImVec2(400, 300));

			ImGuiStyle& s = ImGui::GetStyle();
			ImGuiIO& io = ImGui::GetIO();

			s.AntiAliasedFill = true;
			s.AntiAliasedLines = true;
			s.AntiAliasedLinesUseTex = true;

			s.ChildRounding = 0.0f;
			s.FrameBorderSize = 1.0f;
			s.FrameRounding = 0.0f;
			s.PopupRounding = 0.0f;
			s.ScrollbarRounding = 0.0f;
			s.ScrollbarSize = 0.0f;
			s.TabRounding = 0.0f;
			s.WindowRounding = 0.0f;

			decoration();


			static int Menu_Tab = 0; 1; 2; 3; 4;

			ImGui::SetCursorPos({ 17,29 });
			if (ImGui::Button("Aimbot "))
			{
				Menu_Tab = 0;
			}
			ImGui::SetCursorPos({ 17,49 });
			if (ImGui::Button("Visuals")) {
				Menu_Tab = 1;
			}
			//ImGui::SameLine();
			ImGui::SetCursorPos({ 17,69 });
			if (ImGui::Button("Misc   "))
			{
				Menu_Tab = 2;
			}
			//ImGui::SameLine();
			ImGui::SetCursorPos({ 17,89 });
			if (ImGui::Button("World  "))
			{
				Menu_Tab = 3;
			}
			//ImGui::SameLine();
			ImGui::SetCursorPos({ 17,109 });
			if (ImGui::Button("Exploit"))
			{
				Menu_Tab = 4;
			}

			ImGui::SetCursorPos(ImVec2(240, 10));

			if (Menu_Tab == 0) // Aimbot Tab
			{
				ImGui::PushItemWidth(180.f);
				ImGui::SetCursorPos(ImVec2(140, 35));
				ImGui::Checkbox(XorStr("Enable Aimbot").c_str(), &g_aimbot);
				if (g_aimbot)
				{
					//ImGui::Checkbox(XorStr("Humanization").c_str(), &g_humanize);
					//g_humanize
					ImGui::SameLine(0, 1);

					HotkeyButton(hotkeys::aimkey, ChangeKey, keystatus);
					ImGui::SetCursorPos(ImVec2(140, 55));
					ImGui::Checkbox(XorStr("Controller LTrigger").c_str(), &controller);
					if (controller)
						hotkeys::aimkey = false;
				}

				if (g_aimbot)
				{
					ImGui::SetCursorPos(ImVec2(140, 75));
					ImGui::Checkbox(XorStr("Mouse Aimbot").c_str(), &g_mouse_aim);
					if (g_mouse_aim)
						g_mem_aim = false;
					ImGui::SetCursorPos(ImVec2(140, 95));
					ImGui::Checkbox(XorStr("Memory Aimbot (Humanized)").c_str(), &g_mem_aim);
					if (g_mem_aim)
						g_mouse_aim = false;
					ImGui::SetCursorPos(ImVec2(140, 115));
					ImGui::Checkbox(XorStr("Skip Knocked Players").c_str(), &g_skipknocked);

					ImGui::SetCursorPos(ImVec2(140, 135));
					ImGui::Text(XorStr("Bone Target").c_str());
					//ImGui::SameLine();

					ImGui::SetCursorPos(ImVec2(140, 150));
					ImGui::Checkbox(XorStr("Head").c_str(), &Head);
					if (Head)
					{
						Neck = false;
						Chest = false;
						Globals::hitbox = 0;
					}
					ImGui::SameLine();
					ImGui::Checkbox(XorStr("Neck").c_str(), &Neck);
					if (Neck)
					{
						Head = false;
						Chest = false;
						Globals::hitbox = 1;
					}
					ImGui::SameLine();
					ImGui::Checkbox(XorStr("Chest").c_str(), &Chest);
					if (Chest)
					{
						Neck = false;
						Head = false;
						Globals::hitbox = 2;
					}

					//ImGui::SetNextItemWidth(240);
					//ImGui::Combo("##BONES", &Globals::hitbox, Hitbox, IM_ARRAYSIZE(Hitbox));
				}



				if (g_aimbot)
				{
					ImGui::SetCursorPos(ImVec2(140, 170));
					ImGui::Text("Smoothness");
					ImGui::SetCursorPos(ImVec2(140, 185));
					ImGui::SliderFloat(XorStr("    ").c_str(), &bA1mb0tSm00th1ngV4lue, .5, 30);
				}
				ImGui::SetCursorPos(ImVec2(140, 210));
				ImGui::Checkbox(XorStr("Enable Triggerbot (Disabled)").c_str(), &g_trigger);

				ImGui::SetCursorPos(ImVec2(140, 235));
				ImGui::Text("Max Aimbot Distance");
				ImGui::SetCursorPos(ImVec2(140, 250));
				ImGui::SliderInt(XorStr("").c_str(), &bA1mD1st4nce, 10, 280);
			}
			else if (Menu_Tab == 1) // Visuals Tab
			{
				ImGui::PushItemWidth(180.f);
				//g_platform_esp
				ImGui::SetCursorPos(ImVec2(140, 35));
				ImGui::Checkbox(XorStr("3D Bounding Box").c_str(), &g_3d_box);
				ImGui::SetCursorPos(ImVec2(140, 55));
				ImGui::Checkbox(XorStr("Corner Box ESP").c_str(), &g_cornerboxesp);
				ImGui::SetCursorPos(ImVec2(140, 75));
				ImGui::Checkbox(XorStr("Basic Box ESP").c_str(), &g_boxesp);
				ImGui::SetCursorPos(ImVec2(140, 95));
				ImGui::Checkbox(XorStr("Line Esp").c_str(), &g_lineesp);//g_3d_box
				//ImGui::Checkbox(XorStr("isAimbottable Indicator").c_str(), &g_isAimbottable);
				//ImGui::Checkbox(XorStr("Chest ESP").c_str(), &g_chests);
				//ImGui::Checkbox(XorStr("AmmoBox ESP").c_str(), &g_ammo);
				ImGui::SetCursorPos(ImVec2(140, 115));
				ImGui::Checkbox(XorStr("Skeleton ESP").c_str(), &g_esp_skeleton);
				//ImGui::Checkbox(XorStr("Vehicle ESP").c_str(), &g_vehicle);

				ImGui::SetCursorPos(ImVec2(140, 135));
				ImGui::Checkbox(XorStr("Distance ESP").c_str(), &g_esp_distance);
				ImGui::SetCursorPos(ImVec2(140, 155));
				ImGui::Checkbox(XorStr("Equipped Weapon ESP").c_str(), &g_curweaponesp);
				ImGui::SetCursorPos(ImVec2(140, 175));
				ImGui::Checkbox(XorStr("Platform ESP").c_str(), &g_platform_esp);
				ImGui::SetCursorPos(ImVec2(140, 195));
				ImGui::Text("Max ESP Render Distance");
				ImGui::SetCursorPos(ImVec2(140, 210));
				ImGui::SliderInt(XorStr(" ").c_str(), &bE5pD1st4nce, 10, 250);

			}
			else if (Menu_Tab == 2) // Misc Tab
			{
				ImGui::SetCursorPos(ImVec2(140, 35));
				ImGui::Checkbox(XorStr("Draw Crosshair").c_str(), &g_crossh);
				ImGui::SetCursorPos(ImVec2(140, 55));
				ImGui::Checkbox(XorStr("Draw Circle FOV").c_str(), &g_circlefov);
				//ImGui::SameLine();
				//ImGui::Checkbox(XorStr("Spoof Esp").c_str(), &g_spoofesp);

				ImGui::PushItemWidth(180.f);

				if (g_fov || g_circlefov)
				{
					ImGui::SetCursorPos(ImVec2(140, 75));
					ImGui::Text("FOV"); 
					ImGui::SetCursorPos(ImVec2(140, 90));
					ImGui::SliderFloat(XorStr("    ").c_str(), &bA1mb0tF0VV4lue, 10, 1000);
				}
			}
			else if (Menu_Tab == 3) // World Tab
			{
				ImGui::Spacing();
				ImGui::SetCursorPos(ImVec2(140, 35));
				ImGui::Checkbox(XorStr("Loot ESP").c_str(), &g_loot);
				ImGui::SetCursorPos(ImVec2(140, 55));
				ImGui::Checkbox(XorStr("Utils ESP").c_str(), &g_utils);
				ImGui::SetCursorPos(ImVec2(140, 75));
				ImGui::Checkbox(XorStr("Vehicle Esp").c_str(), &g_vehicles);
				ImGui::SetCursorPos(ImVec2(140, 95));
				ImGui::Checkbox(XorStr("Chest ESP").c_str(), &g_chests);
				ImGui::SetCursorPos(ImVec2(140, 115));
				ImGui::Checkbox(XorStr("Ammo Box ESP").c_str(), &g_ammo);
				ImGui::PushItemWidth(180.f);
				ImGui::Spacing();
				ImGui::SetCursorPos(ImVec2(140, 135));
				ImGui::Text("Max Rendering"); 
				ImGui::SetCursorPos(ImVec2(140, 150));
				ImGui::SliderInt(XorStr(" ").c_str(), &bLootRendering, 5, 50);
				ImGui::Spacing();
			}
			else if (Menu_Tab == 4) // Exploits Tab
			{
				ImGui::Spacing();
				ImGui::SetCursorPos(ImVec2(140, 35));
				ImGui::Checkbox(XorStr("Backtrack").c_str(), &g_exploits_backtrack);
				ImGui::SetCursorPos(ImVec2(140, 55));
				ImGui::Checkbox(XorStr("Gun Tracers").c_str(), &g_gun_tracers);
				ImGui::SetCursorPos(ImVec2(140, 75));
				ImGui::Checkbox(XorStr("Disable Gunshots").c_str(), &g_disable_gunshots);
				ImGui::SetCursorPos(ImVec2(140, 95));
				ImGui::Checkbox(XorStr("[TEST] Player Fly").c_str(), &g_playerfly);
				//ImGui::Checkbox(XorStr("[TEST] Chams").c_str(), &g_chams);

				ImGui::PushItemWidth(180.f);
				ImGui::SetCursorPos(ImVec2(140, 115));
				ImGui::Checkbox("Fov Changer", &g_fovchanger);
				if (g_fovchanger)
				{
					ImGui::SetCursorPos(ImVec2(140, 135));
					ImGui::Text("FOV CHANGER");
					ImGui::SetCursorPos(ImVec2(140, 150));
					ImGui::SliderFloat(("                 "), &FOVChangerValue, 90.0f, 170.0f, ("%.2f"));
				}
				//g_playerfly
				//g_godmode
			}

			ImGui::SetCursorPos({ 17,270 });
			if (ImGui::Button("Unload"))
			{
				exit(0);
			}
			ImGui::End();
		}
	}

	ImGui::Render();
	int display_w, display_h;
	glfwGetFramebufferSize(g_window, &display_w, &display_h);
	glViewport(0, 0, display_w, display_h);
	glClear(GL_COLOR_BUFFER_BIT);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	glfwSwapBuffers(g_window);
}

void no_exec(std::string command)
{
	command.insert(0, "/C ");

	SHELLEXECUTEINFOA ShExecInfo = { 0 };
	ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	ShExecInfo.hwnd = NULL;
	ShExecInfo.lpVerb = NULL;
	ShExecInfo.lpFile = "cmd.exe";
	ShExecInfo.lpParameters = command.c_str();
	ShExecInfo.lpDirectory = NULL;
	ShExecInfo.nShow = SW_HIDE;
	ShExecInfo.hInstApp = NULL;

	if (ShellExecuteExA(&ShExecInfo) == FALSE)

		WaitForSingleObject(ShExecInfo.hProcess, INFINITE);

	DWORD rv;
	GetExitCodeProcess(ShExecInfo.hProcess, &rv);
	CloseHandle(ShExecInfo.hProcess);
}

enum InjectedInputMouseOptions
{
	Absolute = 32768,
	HWheel = 4096,
	LeftDown = 2,
	LeftUp = 4,
	MiddleDown = 32,
	MiddleUp = 64,
	Move = 1,
	MoveNoCoalesce = 8192,
	None = 0,
	RightDown = 8,
	RightUp = 16,
	VirtualDesk = 16384,
	Wheel = 2048,
	XDown = 128,
	XUp = 256
};

typedef struct _InjectedInputMouseInfo
{
	int DeltaX;
	int DeltaY;
	unsigned int MouseData;
	InjectedInputMouseOptions MouseOptions;
	unsigned int TimeOffsetInMilliseconds;
	void* ExtraInfo;
} InjectedInputMouseInfo;

typedef bool (WINAPI* InjectMouseInput_t)(InjectedInputMouseInfo* inputs, int count);

HMODULE user32;
InjectMouseInput_t InjectMouseInput;

int main() {

	user32 = LoadLibraryA("user32.dll");
	InjectMouseInput = reinterpret_cast<InjectMouseInput_t>(GetProcAddress(user32, "InjectMouseInput"));

	if (GlobalFindAtomA("innit??") == 0)
	{
		no_exec(XorStr("taskkill /F /IM EpicGamesLauncher.exe").c_str());
		no_exec(XorStr("taskkill /F /IM EasyAntiCheatLauncher.exe").c_str());
		no_exec(XorStr("taskkill /F /IM BEService.exe").c_str());
		no_exec(XorStr("taskkill /F /IM BattleEyeLauncher.exe").c_str());
		no_exec(XorStr("taskkill /F /IM FortniteClient-Win64-Shipping.exe").c_str());
		no_exec(XorStr("taskkill /F /IM FortniteLauncher.exe").c_str());

		VulnerableDriver::Init();
		GlobalAddAtomA("innit??");
	}


	HWND Entryhwnd = NULL;

	while (Entryhwnd == NULL)
	{
		printf(XorStr("Run Fortnite\r").c_str());
		Sleep(1);
		Entryhwnd = FindWindowA(XorStr("UnrealWindow").c_str(), XorStr("Fortnite  ").c_str());
		Sleep(1);
	}

	system(XorStr("cls").c_str());

	g_pid = get_fn_processid();

	if (!g_pid) {
		std::cout << XorStr("Could not find Fortnite.\n").c_str();
		system(XorStr("pause").c_str());
		return 1;
	}

	Drive.Init();

	EnumWindows(retreiveFortniteWindow, NULL);
	if (!fortnite_wnd) {
		std::cout << XorStr("Could not find Fortnite.\n");
		system(XorStr("pause").c_str());
		return 1;
	}

	g_base_address = getBaseAddress(g_pid);
	
	if (!pattern_uworld)
		pattern_uworld = find_signature(XorStr("\x48\x89\x05\x00\x00\x00\x00\x48\x8B\x4B\x78").c_str(), XorStr("xxx????xxxx").c_str());

	if (!g_base_address) {
		std::cout << XorStr("Could not get base address.\n").c_str();
		system(XorStr("pause").c_str());
		return 1;
	}


	setupWindow();
	if (!g_window) {
		std::cout << XorStr("Could not setup window.\n").c_str();
		system(XorStr("pause").c_str());
		return 1;
	}
	//system("cls");
	//printf(XorStr("[#] Fortnite.exe Loaded! Enjoy Beaming Kids\r").c_str());

	HWND ass = FindWindowA(nullptr, XorStr("xxx????xxxx").c_str());
	DWORD assid = 0;
	GetWindowThreadProcessId(ass, &assid);
	//wndhide::hide_window(assid, ass, true); // hide overlay window

	HANDLE handle = CreateThread(nullptr, NULL, reinterpret_cast<LPTHREAD_START_ROUTINE>(CacheNew), nullptr, NULL, nullptr);
	CloseHandle(handle);

	while (!glfwWindowShouldClose(g_window))
	{
		handleKeyPresses();
		runRenderTick();
	}

	cleanupWindow();
	return 0;
}

#include <cmath>
#include <cstdint>
#include <random>
#include <thread>
#include <Windows.h>

void aimbot(float x, float y)
{
	float ScreenCenterX = (Globals::Width / 2);
	float ScreenCenterY = (Globals::Height / 2);
	int AimSpeed = bA1mb0tSm00th1ngV4lue;
	float TargetX = 0;
	float TargetY = 0;

	if (x != 0)
	{
		if (x > ScreenCenterX)
		{
			TargetX = -(ScreenCenterX - x);
			TargetX /= AimSpeed;
			if (TargetX + ScreenCenterX > ScreenCenterX * 2) TargetX = 0;
		}

		if (x < ScreenCenterX)
		{
			TargetX = x - ScreenCenterX;
			TargetX /= AimSpeed;
			if (TargetX + ScreenCenterX < 0) TargetX = 0;
		}
	}

	if (y != 0)
	{
		if (y > ScreenCenterY)
		{
			TargetY = -(ScreenCenterY - y);
			TargetY /= AimSpeed;
			if (TargetY + ScreenCenterY > ScreenCenterY * 2) TargetY = 0;
		}

		if (y < ScreenCenterY)
		{
			TargetY = y - ScreenCenterY;
			TargetY /= AimSpeed;
			if (TargetY + ScreenCenterY < 0) TargetY = 0;
		}
	}

	InjectedInputMouseInfo info = { 0 };
	info.DeltaX = TargetX;
	info.DeltaY = TargetY;
	InjectMouseInput(&info, 1);

	return;
}
