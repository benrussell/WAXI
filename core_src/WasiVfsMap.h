
#include "WasmVM_Config.h"

#include <string>
#include <vector>

#include <iostream>

#include <wasmtime.hh>


#include <nlohmann/json.hpp>


struct VFS_record{
	std::string target;
	std::string mount;

	// Constructor to initialize target and mount
	VFS_record(const std::string& target, const std::string& mount)
		: target(target), mount(mount) {}
};

struct VFS_record_set{
	std::vector<VFS_record> rs;
};



using namespace wasmtime;


class WasiVfsMap{
    public:
    WasiVfsMap( 
        WasmVM_Config path_config,
        WasiConfig* wasi
    ){

		std::cout << "waxi/  VFS  {plugin_root}: " << path_config.plugin_folder << std::endl;
		std::cout << "waxi/  VFS  {xp_root}    : " << path_config.xp_folder << std::endl;
		std::cout << "waxi/  VFS  {acf_root}   : " << path_config.acf_folder << std::endl;

        /*
        VFS definition in the config.json looks like this:

        "vfs":[
                ["{plugin_root}/vfs_root/", "/"],

                ["{xp_root}/", "/X-Plane"],
                ["{plugin_root}/vfs_lock/", "/X-Plane/Aircraft"],
                ["{plugin_root}/vfs_lock/", "/X-Plane/Resources/plugins"],
                
                ["{acf_root}/", "/Aircraft"],
                ["{plugin_root}/vfs_lock/", "/Aircraft/plugins"]
            ],

            - {plugin_root}
            - {xp_root}
            - {acf_root}
        */



		//need to load this from json?
		VFS_record_set vfs_fstab_tpl;
		

		std::string config_json_filename = path_config.plugin_folder + "/config.json";
		//std::cout << "waxi/ WasiVfsMap/ dyn config json fn: " + config_json_filename + "\n";

		// Load configuration from config.json
		std::ifstream configFile(config_json_filename);
		if (configFile.is_open()) {
			try {
				nlohmann::json jsonConfig;
				configFile >> jsonConfig;
				if (jsonConfig.contains("vfs") && jsonConfig["vfs"].is_array()) {
					for (const auto& vfs_entry : jsonConfig["vfs"]) {
						if (vfs_entry.is_array() && vfs_entry.size() == 2) {
							std::string target = vfs_entry[0].get<std::string>();
							std::string mount = vfs_entry[1].get<std::string>();
							vfs_fstab_tpl.rs.emplace_back(VFS_record(target, mount));
						} else {
							std::cerr << "waxi/ Invalid VFS entry format in config.json\n";
						}
					}
				} else {
					throw std::runtime_error("waxi/ Missing or invalid 'vfs' definition in config.json");
				}


				//std::cout << "waxi/ Loaded configuration from config.json\n";
			} catch (const std::exception &e) {
				std::cerr << "waxi/ Error parsing config.json: " << e.what() << "\n";
				throw std::runtime_error(std::string("waxi/ Error parsing config.json: ") + e.what());
				
			}
		} else {
			std::cerr << "waxi/ Could not open config.json\n";
			throw std::runtime_error("waxi/ Could not open config.json\n");
		}

		

		VFS_record_set vfs_fstab;
		
		for (const auto &record : vfs_fstab_tpl.rs) {
			std::string target = record.target;
			std::string mount = record.mount;

			// Replace {plugin_root}, {xp_root}, and {acf_root} with actual paths
			size_t pos;
			while ((pos = target.find("{plugin_root}")) != std::string::npos) {
				target.replace(pos, std::string("{plugin_root}").length(), path_config.plugin_folder);
			}
			while ((pos = target.find("{xp_root}")) != std::string::npos) {
				target.replace(pos, std::string("{xp_root}").length(), path_config.xp_folder);
			}
			while ((pos = target.find("{acf_root}")) != std::string::npos) {
				target.replace(pos, std::string("{acf_root}").length(), path_config.acf_folder);
			}

			vfs_fstab.rs.emplace_back(VFS_record{target, mount});
		}


        // call wasi with preopen_dir calls
		for (const auto &record : vfs_fstab.rs) {
			//std::cout << "waxi/  VFS  " << record.target << " -> " << record.mount << std::endl;
			std::cout << "waxi/  VFS  " << std::left << std::setw(40) << record.target 
					  << " :: " << record.mount << std::endl;

			if( ! wasi->preopen_dir(record.target, record.mount) ){
				const std::string err_msg = "waxi/ Error failed to open VFS folder: [" + record.target + "]";
				std::cout << err_msg + "\n";
				throw std::runtime_error( err_msg );
			}			
		}

    };
};



