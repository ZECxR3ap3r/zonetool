// ======================= ZoneTool =======================
// zonetool, a fastfile linker for various
// Call of Duty titles. 
//
// Project: https://github.com/ZoneTool/zonetool
// Author: RektInator (https://github.com/RektInator)
// License: GNU GPL v3.0
// ========================================================
#include "stdafx.hpp"
#include <Dbghelp.h>

// include zonetool linkers
#include <IW3/IW3.hpp>
#include <IW4/IW4.hpp>
#include <IW5/IW5.hpp>
#include <CODO/CODO.hpp>
#include "Utils/Swizzle.hpp"

#pragma comment(lib, "Dbghelp")

std::string currentzone;

namespace ZoneTool
{
	std::vector<std::shared_ptr<ILinker>> linkers;
	std::map<std::string, std::function<void(std::vector<std::string>)>> commands;

	void register_command(const std::string& name, std::function<void(std::vector<std::string>)> cb)
	{
		commands[name] = cb;
	}

	void execute_command(std::vector<std::string> args)
	{
		const auto itr = commands.find(args[0]);
		if (itr != commands.end())
		{
			itr->second(args);
		}
		else
		{
			ZONETOOL_ERROR("Unknown command \"%s\".", args[0].data());
		}
	}

	void command_thread()
	{
		while (true)
		{
			// Get console input
			std::string input;
			std::getline(std::cin, input);

			std::vector<std::string> args;

			// Load arguments into vector
			if (input.find(' ') != std::string::npos)
			{
				args = split(input, ' ');
			}
			else
			{
				args.push_back(input);
			}

			// Execute command
			execute_command(args);
		}
	}

	void add_assets_using_iterator(const std::string& fastfile, const std::string& type, const std::string& folder,
		const std::string& extension, bool skip_reference, IZone* zone)
	{
		if (std::filesystem::is_directory("zonetool\\" + fastfile + "\\" + folder))
		{
			for (auto& file : std::filesystem::recursive_directory_iterator(
				"zonetool\\" + fastfile + "\\" + folder))
			{
				if (is_regular_file(file))
				{
					auto filename = file.path().filename().string();

					if (skip_reference && filename[0] == ',')
					{
						continue;
					}

					if (filename.length() > extension.length() &&
						filename.substr(filename.length() - extension.length()) == extension)
					{
						filename = filename.substr(0, filename.length() - extension.length());

						zone->add_asset_of_type(type, filename);
					}
				}
			}
		}
	}

	void add_assets_with_relative_path(const std::string& fastfile,
		const std::string& type,
		const std::string& folder,
		const std::string& extension,
		bool skip_reference,
		IZone* zone,
		bool includeFolderInName = true)
	{
		std::string baseFolder = type + "s";
		std::filesystem::path basePath = std::filesystem::path("zonetool") / fastfile / baseFolder / folder;

		if (!std::filesystem::is_directory(basePath))
		{
			ZONETOOL_INFO("Folder does not exist: %s\n", basePath.string().c_str());
			return;
		}

		for (auto& file : std::filesystem::recursive_directory_iterator(basePath))
		{
			if (!std::filesystem::is_regular_file(file))
				continue;

			std::filesystem::path relativePath = std::filesystem::relative(file.path(), basePath);

			std::string assetName;
			if (includeFolderInName || type == "material")
				assetName = (std::filesystem::path(folder) / relativePath).string();
			else
				assetName = relativePath.string();

			std::replace(assetName.begin(), assetName.end(), '\\', '/');

			if (skip_reference && !assetName.empty() && assetName[0] == ',')
				continue;

			if (!extension.empty() && assetName.length() > extension.length() &&
				assetName.substr(assetName.length() - extension.length()) == extension)
			{
				assetName = assetName.substr(0, assetName.length() - extension.length());
			}

			ZONETOOL_INFO("Folder Parsing: %s", assetName.c_str());

			zone->add_asset_of_type(type, assetName);
		}
	}

	bool parse_localizedstrings_file(IZone* zone, const std::string& fastfile, const std::string& file)
	{
		auto* fp = FileSystem::FileOpen(file, "rb");
		if (fp)
		{
			ZONETOOL_INFO("Parsing localizedstrings \"%s\"...", file.c_str());

			auto bytes = FileSystem::ReadBytes(fp, FileSystem::FileSize(fp));
			std::string str = std::string(bytes.begin(), bytes.end());

			std::stringstream ss(str);
			std::string s;

			char* data;
			size_t size = 0;

			bool failed = false;
			std::string name;
			std::string value;
			size_t line = 0;
			size_t i = 0;
			while (getline(ss, s))
			{
				data = s.data();
				size = s.size();
				++line;
				if (size < 2)
					continue;
				for (i = 0; i < size; i++)
				{
					if (i + 1 < size && data[i] == '/' && data[i + 1] == '/')
						break;
					if (isspace(data[i]))
						continue;
					if (data[i] >= 'A' && data[i] <= 'Z')
					{
						if (!strncmp(&data[i], "REFERENCE", 9))
						{
							i += 9;
							while (isspace(data[i]))
							{
								if (i >= size)
								{
									failed = true;
									break;
								}
								i++;
							}
							if (failed)
							{
								break;
							}

							name.clear();
							while (!isspace(data[i]))
							{
								if (i >= size)
								{
									break;
								}
								name += data[i];
								i++;
							}
							break;
						}
						if (!strncmp(&data[i], "LANG_", 5))
						{
							i += 5;
							while (data[i] != '"')
							{
								if (i >= size)
								{
									failed = true;
									break;
								}
								i++;
							}
							if (failed)
							{
								break;
							}

							i++;
							value.clear();
							while (data[i] != '"')
							{
								if (i >= size)
								{
									failed = true;
									break;
								}
								if(data[i] == '\\' && i + 1 < size)
								{
									switch (data[i + 1])
									{
									case 'n':
										value += '\n';
										i += 2;
										break;
									case 't':
										value += '\t';
										i += 2;
										break;
									default:
										value += '\\';
										i++;
										break;
									}
								}
								else
								{
									value += data[i];
									i++;
								}
							}
							break;
						}
						if (!strncmp(&data[i], "ENDMARKER", 9))
						{
							FileSystem::FileClose(fp);
							return true;
						}
					}
				}
				if (failed)
				{
					ZONETOOL_WARNING("\"%s\" parse failed at line: %zu index: %zu", file.data(), line, i);
					FileSystem::FileClose(fp);
					return false;
				}
				if (!name.empty() && !value.empty())
				{
					struct LocalizeStruct
					{
						const char* value;
						const char* name;
					};
					auto loc = new LocalizeStruct;
					loc->name = _strdup(name.data());
					loc->value = _strdup(value.data());

					auto type = zone->get_type_by_name("localize");
					if (type == -1)
					{
						ZONETOOL_ERROR("Could not translate typename localize to an integer!");
					}
					try
					{
						zone->add_asset_of_type_by_pointer(type, loc);
					}
					catch (std::exception& ex)
					{
						ZONETOOL_FATAL("A fatal exception occured while building zone \"%s\", exception was: %s", fastfile.data(), ex.what());
					}
					name.clear();
					value.clear();
				}
			}
			FileSystem::FileClose(fp);
			return true;
		}
		return false;
	}

	std::string get_output_filename(const std::string& zone_name) {
		if (zone_name != "patch_mp" && zone_name != "ui_mp" && !zone_name.starts_with("mp_"))
			return "mod";

		return zone_name;
	}

	void parse_csv_file(ILinker* linker, IZone* zone, const std::string& fastfile, const std::string& csv_file)
	{
		auto path = "zone_source\\" + csv_file + ".csv";
		auto* parser = CsvParser_new(path.data(), ",", false);

		if (!parser)
		{
			ZONETOOL_ERROR("Could not find csv file \"%s\" to build zone!", csv_file.data());
			return;
		}

		auto is_referencing = false;
		auto* row = CsvParser_getRow(parser);
		while (row != nullptr)
		{
			// parse options
			if ((strlen(row->fields_[0]) >= 1 && row->fields_[0][0] == '#') || (strlen(row->fields_[0]) >= 2 && row->
				fields_[0][0] == '/' && row->fields_[0][1] == '/'))
			{
				// comment line, go to next line.
				goto nextRow;
			}
			if (!strlen(row->fields_[0]))
			{
				// empty line, go to next line.
				goto nextRow;
			}
			if (row->fields_[0] == "require"s)
			{
				linker->load_zone(row->fields_[1]);
			}
			else if (row->fields_[0] == "include"s)
			{
				parse_csv_file(linker, zone, fastfile, row->fields_[1]);
			}
			else if (row->fields_[0] == "reference"s)
			{
				if (row->numOfFields_ >= 2)
				{
					is_referencing = row->fields_[1] == "true"s;
				}
			}
			else if (row->fields_[0] == "map"s)
			{
				zone->add_asset_of_type("techset", "wc_l_hsm_r0c0n0s0");
			}
			else if (row->fields_[0] == "iterate"s)
			{
				try
				{
					add_assets_using_iterator(FileSystem::GetFastFile(), "fx", "fx", ".fxe", true, zone);
					add_assets_using_iterator(FileSystem::GetFastFile(), "xanimparts", "XAnim", ".xae2", true, zone);
					add_assets_using_iterator(FileSystem::GetFastFile(), "xmodel", "XModel", ".xme6", true, zone);
				}
				catch (std::exception& ex)
				{
					ZONETOOL_FATAL("A fatal exception occured while building zone \"%s\", exception was: %s\n", fastfile.data(), ex.what());
				}
			}
			else if (row->fields_[0] == "add_folder"s)
			{
				try
				{
					std::string folder = row->fields_[2];

					if(row->fields_[1] == "material")
						add_assets_with_relative_path(FileSystem::GetFastFile(), row->fields_[1], folder, "", true, zone, true);
					else
						add_assets_with_relative_path(FileSystem::GetFastFile(), row->fields_[1], folder, "", true, zone, false);
				}
				catch (const std::exception& ex)
				{
					ZONETOOL_FATAL(
						"A fatal exception occured while building zone \"%s\", exception was: %s\n",
						fastfile.data(), ex.what()
					);
				}
			}
			else if (row->fields_[0] == "iterate_techsets"s)
			{
				try
				{
					add_assets_using_iterator(FileSystem::GetFastFile(), "techset", "techsets", ".techset", true, zone);
				}
				catch (std::exception& ex)
				{
					ZONETOOL_FATAL("A fatal exception occured while building zone \"%s\", exception was: %s\n", fastfile.data(), ex.what());
				}
			}
			else if (row->fields_[0] == "forceExternalAssets"s)
			{
				ZONETOOL_WARNING("forceExternalAssets has been turned on!");
				FileSystem::ForceExternalAssets(true);
			}
			else
			{
				if (row->fields_[0] == "localize"s && row->numOfFields_ >= 2 && 
					FileSystem::FileExists("localizedstrings/"s + row->fields_[1] + ".str"))
				{
					parse_localizedstrings_file(zone, FileSystem::GetFastFile(), "localizedstrings/"s + row->fields_[1] + ".str");
				}
				else
				{
					if (row->numOfFields_ >= 2)
					{
						if (linker->is_valid_asset_type(row->fields_[0]))
						{
							try
							{
								zone->add_asset_of_type(
									row->fields_[0],
									((is_referencing) ? ","s : ""s) + row->fields_[1]
								);
							}
							catch (std::exception& ex)
							{
								ZONETOOL_FATAL("A fatal exception occured while building zone \"%s\", exception was: %s\n", fastfile.data(), ex.what());
							}
						}
					}
				}
			}

		nextRow:
			// destroy row and alloc next one.
			CsvParser_destroy_row(row);
			row = CsvParser_getRow(parser);
		}

		// free csv parser
		CsvParser_destroy(parser);
	}
	
	void build_zone(ILinker* linker, const std::string& zone_name)
	{
		const std::string output_file = get_output_filename(zone_name);

		FileSystem::SetFastFile(zone_name);

		ZONETOOL_INFO("Building fastfile \"%s\" as output \"%s\" for game \"%s\"", zone_name.c_str(), output_file.c_str(), linker->version());

		auto zone = linker->alloc_zone("mod");
		if (zone == nullptr)
		{
			ZONETOOL_ERROR("An error occurred while building fastfile \"%s\"", zone_name.c_str());
			return;
		}

		zone->set_target(zone_target::pc);

		if (linker->version() == "IW4"s)
			zone->set_target_version(zone_target_version::iw4_release);
		else if (linker->version() == "IW5"s)
			zone->set_target_version(zone_target_version::iw5_release);

		// Parse CSV using the internal zone name
		parse_csv_file(linker, zone.get(), output_file, zone_name);

		// Allocate zone buffer
		auto buffer = linker->alloc_buffer();

		// Branding asset still uses the internal zone name
		zone->add_asset_of_type("rawfile", output_file);

		// Compile the zone (writes to mod.ff instead of zone_name.ff)
		zone->build(buffer.get());
	}

	ILinker* current_linker;

	LONG NTAPI exception_handler(_EXCEPTION_POINTERS* info)
	{
		if (info->ExceptionRecord->ExceptionCode == STATUS_INTEGER_OVERFLOW ||
			info->ExceptionRecord->ExceptionCode == STATUS_FLOAT_OVERFLOW ||
			info->ExceptionRecord->ExceptionCode == 0x406D1388 || 
			info->ExceptionRecord->ExceptionCode == STATUS_BREAKPOINT ||
			info->ExceptionRecord->ExceptionCode == 0x40010006)
		{
			return EXCEPTION_CONTINUE_EXECUTION;
		}
		
		if (ZONETOOL_VERSION == "0.0.0"s)
		{
			MessageBoxA(nullptr, va("An exception occured (0x%08X) and ZoneTool must be restarted to continue. However, ZoneTool has detected that you are using a custom DLL. If you want to submit an issue, try to reproduce the bug with the latest release of ZoneTool. The latest version can be found here: https://github.com/ZoneTool/zonetool/releases", info->ExceptionRecord->ExceptionCode).data(), "ZoneTool", MB_ICONERROR);
			std::exit(0);
		}
		
		std::filesystem::create_directories("zonetool/crashdumps");

		const auto exception_time = std::time(nullptr);
		const auto linker_name = (current_linker) ? current_linker->version() : "unknown";
		const auto file_name = va("zonetool/crashdumps/zonetool-exception-%s-%s-%llu.dmp", linker_name, ZONETOOL_VERSION, exception_time);
		
		DWORD dump_type = MiniDumpIgnoreInaccessibleMemory;
		dump_type |= MiniDumpWithHandleData;
		dump_type |= MiniDumpScanMemory;
		dump_type |= MiniDumpWithProcessThreadData;
		dump_type |= MiniDumpWithFullMemoryInfo;
		dump_type |= MiniDumpWithThreadInfo;
		dump_type |= MiniDumpWithCodeSegs;
		dump_type |= MiniDumpWithDataSegs;
		
		const DWORD file_share = FILE_SHARE_READ | FILE_SHARE_WRITE;
		const HANDLE file_handle = CreateFileA(file_name.data(), GENERIC_WRITE | GENERIC_READ, file_share, nullptr, (file_share & FILE_SHARE_WRITE) > 0 ? OPEN_ALWAYS : OPEN_EXISTING, NULL, nullptr);
		MINIDUMP_EXCEPTION_INFORMATION ex = { GetCurrentThreadId(), info, FALSE };
		if (!MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), file_handle, static_cast<MINIDUMP_TYPE>(dump_type), &ex, nullptr, nullptr))
		{
			
		}

		const auto message = va("An exception occured and ZoneTool must be restarted to continue. If this keeps happening, create an issue on https://github.com/ZoneTool/zonetool with the crashdump attached. The crashdump can be found at: \"%s\".", file_name.data());
		MessageBoxA(nullptr, message.data(), "ZoneTool", MB_ICONERROR);
		std::exit(0);
	}
	
	void create_console()
	{
#ifdef USE_VMPROTECT
		VMProtectBeginUltra("CreateConsole");
#endif

		if (!IsDebuggerPresent())
		{
			// Catch exceptions
			AddVectoredExceptionHandler(TRUE, exception_handler);
		}
		
		// Allocate console
		AllocConsole();
		freopen("CONIN$", "r", stdin);
		freopen("CONOUT$", "w", stdout);
		freopen("CONOUT$", "w", stderr);

		// Set console name
		SetConsoleTitleA("Faggot Tool");

		// Spawn command thread
		CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(command_thread), nullptr, 0, nullptr);

		// Commands
		register_command("quit"s, [](std::vector<std::string>)
		{
			ExitProcess(0);
		});
		auto buildzone = [](std::vector<std::string> args)
		{
			// Check if enough arguments have been passed to the command
			if (args.size() == 1)
			{
				ZONETOOL_ERROR("usage: buildzone <zone>");
				return;
			}

			if (current_linker)
			{
				if(current_linker->supports_building())
				{
					build_zone(current_linker, args[1]);
				}
				else
				{
					ZONETOOL_ERROR("Current linker does not support zone building.");
				}
			}
		};
		register_command("buildzone"s, buildzone);
		register_command("bz"s, buildzone);

		auto loadzone = [](std::vector<std::string> args)
		{
			// Check if enough arguments have been passed to the command
			if (args.size() == 1)
			{
				ZONETOOL_ERROR("usage: loadzone <zone>");
				return;
			}

			// Load zone
			if (current_linker)
			{
				current_linker->load_zone(args[1]);
			}
		};

		register_command("loadzone"s, loadzone);
		register_command("lz"s, loadzone);
		register_command("verifyzone"s, [](std::vector<std::string> args)
		{
			// Check if enough arguments have been passed to the command
			if (args.size() == 1)
			{
				ZONETOOL_ERROR("usage: verifyzone <zone>");
				return;
			}

			// Load zone
			if (current_linker)
			{
				current_linker->verify_zone(args[1]);
			}
		});
		register_command("dumpzone"s, [](std::vector<std::string> args)
		{
			// Check if enough arguments have been passed to the command
			if (args.size() == 1)
			{
				ZONETOOL_ERROR("usage: dumpzone <zone>");
				return;
			}

			// Load zone
			if (current_linker)
			{
				current_linker->dump_zone(args[1]);
			}
		});
		register_command("unloadzones"s, [](std::vector<std::string> args)
		{
			// Unload zones
			if (current_linker)
			{
				current_linker->unload_zones();
			}
	});

#ifdef USE_VMPROTECT
		VMProtectEnd();
#endif
	}

	template <typename T>
	void register_linker()
	{
		linkers.push_back(std::make_shared<T>());
	}

	void branding(ILinker* linker)
	{
	}

	std::vector<std::string> get_command_line_arguments()
	{
		LPWSTR* szArglist;
		int nArgs;

		szArglist = CommandLineToArgvW(GetCommandLineW(), &nArgs);

		std::vector<std::string> args;
		args.resize(nArgs);

		// convert all args to std::string
		for (int i = 0; i < nArgs; i++)
		{
			auto curArg = std::wstring(szArglist[i]);
#pragma warning( push )
#pragma warning( disable : 4244 )
			args[i] = std::string(curArg.begin(), curArg.end());
#pragma warning( pop )
		}

		// return arguments
		return args;
	}

	void handle_params()
	{
		// Wait until the game is loaded
		Sleep(5000);

		// Execute command line commands?
		auto args = get_command_line_arguments();
		if (args.size() > 1)
		{
			for (auto i = 0u; i < args.size(); i++)
			{
				if (i < args.size() - 1)
				{
					if (args[i] == "-buildzone")
					{
						build_zone(current_linker, args[i + 1]);
						i++;
					}
					else if (args[i] == "-loadzone")
					{
						current_linker->load_zone(args[i + 1]);
						i++;
					}
					else if (args[i] == "-dumpzone")
					{
						current_linker->dump_zone(args[i + 1]);
						i++;
					}
				}
			}

			std::exit(0);
		}
	}

	bool is_custom_linker_present()
	{
		return std::filesystem::exists("linker.dll") && std::filesystem::is_regular_file("linker.dll");
	}

	void startup()
	{
#ifdef USE_VMPROTECT
		VMProtectBeginUltra("Startup");
#endif

		// Create stdout console
		create_console();

		// Register linkers
		register_linker<IW3::Linker>();
		register_linker<IW4::Linker>();
		register_linker<IW5::Linker>();
		register_linker<CODO::Linker>();

		// check if a custom linker is present in the current game directory
		if (is_custom_linker_present())
		{
			const auto linker_module = LoadLibraryA("linker.dll");

			if (linker_module != nullptr && linker_module != INVALID_HANDLE_VALUE)
			{
				const auto get_linker_func = GetProcAddress(linker_module, "GetLinker");

				if (get_linker_func != nullptr && get_linker_func != INVALID_HANDLE_VALUE)
				{
					current_linker = Function<ILinker * ()>(get_linker_func)();
				}
			}
		}

		if (!current_linker)
		{
			// Startup compatible linkers
			for (auto& linker : linkers)
			{
				linker->startup();

				if (linker->is_used())
				{
					current_linker = linker.get();
				}
			}
		}

		// Startup complete, show branding
		branding(current_linker);

		// handle startup commands
		CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(handle_params), nullptr, 0, nullptr);

#ifdef USE_VMPROTECT
		VMProtectEnd();
#endif
	}
}
