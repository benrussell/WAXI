
#include "WasmVM_Config.h"

#include <string>
#include <vector>

#include <iostream>

#include <wasmtime.hh>


struct VFS_record{
	std::string target;
	std::string mount;
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

		std::cout << "host/ path_config.plugin_folder: " << path_config.plugin_folder << std::endl;
		std::cout << "host/ path_config.xp_folder: " << path_config.xp_folder << std::endl;
		std::cout << "host/ path_config.acf_folder: " << path_config.acf_folder << std::endl;

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
		vfs_fstab_tpl.rs.emplace_back( VFS_record("{plugin_root}/vfs_root", "/") );

		vfs_fstab_tpl.rs.emplace_back( VFS_record("{xp_root}", "/X-Plane") );
		vfs_fstab_tpl.rs.emplace_back( VFS_record("{plugin_root}/vfs_lock/", "/X-Plane/Aircraft") );
		vfs_fstab_tpl.rs.emplace_back( VFS_record("{plugin_root}/vfs_lock/", "/X-Plane/Resources/plugins") );
		
		vfs_fstab_tpl.rs.emplace_back( VFS_record("{xp_root}/{acf_root}", "/Aircraft") );
		vfs_fstab_tpl.rs.emplace_back( VFS_record("{plugin_root}/vfs_lock/", "/Aircraft/plugins") );
		

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
			std::cout << "host/ VFS: " << record.target << " -> " << record.mount << std::endl;

			if( ! wasi->preopen_dir(record.target, record.mount) ){
				std::cout << "host/ ERROR: Failed to open VFS folder.\n";
				//exit(1); //FIXME: we should probably error out if something fails.
			}			
		}

    };
};



