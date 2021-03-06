
#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <vector>
#include <TlHelp32.h>
#include <stdio.h>


// PatternScan Function 
std::uint8_t* PatternScan(void* module, const char* signature)
{
	static auto pattern_to_byte = [](const char* pattern) {
		auto bytes = std::vector<int>{};
		auto start = const_cast<char*>(pattern);
		auto end = const_cast<char*>(pattern) + strlen(pattern);

		for (auto current = start; current < end; ++current) {
			if (*current == '?') {
				++current;
				if (*current == '?')
					++current;
				bytes.push_back(-1);
			}
			else {
				bytes.push_back(strtoul(current, &current, 16));
			}
		}
		return bytes;
	};

	auto dosHeader = (PIMAGE_DOS_HEADER)module;
	auto ntHeaders = (PIMAGE_NT_HEADERS)((std::uint8_t*)module + dosHeader->e_lfanew);

	auto sizeOfImage = ntHeaders->OptionalHeader.SizeOfImage;
	auto patternBytes = pattern_to_byte(signature);
	auto scanBytes = reinterpret_cast<std::uint8_t*>(module);

	auto s = patternBytes.size();
	auto d = patternBytes.data();

	for (auto i = 0ul; i < sizeOfImage - s; ++i) {
		bool found = true;
		for (auto j = 0ul; j < s; ++j) {
			if (scanBytes[i + j] != d[j] && d[j] != -1) {
				found = false;
				break;
			}
		}
		if (found) {
			return &scanBytes[i];
		}
	}
	return nullptr;
}



int MainThread()
{
	AllocConsole();
	freopen("CONOUT$", "w", stdout);

	HWND CounterStrike = FindWindow(0, "Counter-Strike: Global Offensive");
	DWORD dwPID;
	GetWindowThreadProcessId(CounterStrike, &dwPID);

	static uint8_t* primik = Utils::PatternScan(GetModuleHandleW(L"client_panorama.dll"), "83 F8 05 5E 0F 94 C0 C3") + 2;
		DWORD old_protect = 0;
		uint8_t patch = 0x05;
 
		if (g_Options.misc_primik)
		{
			VirtualProtect(primik, 1, PAGE_EXECUTE_READWRITE, &old_protect);
			patch = 0x00; // change the 3rd byte after cmp instuction so the function returns true
			memcpy(primik, &patch, 1);
			VirtualProtect(primik, 1, old_protect, nullptr);
		}
		else
		{
			VirtualProtect(primik, 1, PAGE_EXECUTE_READWRITE, &old_protect);
			patch = 0x05;
			memcpy(primik, &patch, 1); // return the 3rd byte after the cmp instuction to the original
			VirtualProtect(primik, 1, old_protect, nullptr);
		}

	// fancy stuff
	for (int i = 0; i < 20; i++)
	{
		printf("#");
		Sleep(100);
	}
	printf("\n   PRIME ACTIVATED\n");
	for (int i = 0; i < 20; i++)
	{
		printf("#");
	}
	printf("\n");
	getchar();

	return 0;
}


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
		CreateThread(0, 0, (LPTHREAD_START_ROUTINE)MainThread, 0, 0, 0);
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

