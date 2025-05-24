#ifndef WASMVM_H
#define WASMVM_H

#include <filesystem>
		
#include <fstream>
#include <iostream>
#include <sstream>
#include <wasmtime.hh>
#include <cstring>
#include <iomanip>

#include "xp_api/xp_api.h"

using namespace wasmtime;

#include "WasiVfsMap.h"

#include "WasmVM_Config.h"



class WasmVM
{
public:
	explicit WasmVM(
		std::string &filename,
		WasmVM_Config path_config
	)
	{

		wasmtime::Config config;
		config.consume_fuel(true);
		config.debug_info(true);		   // Enable DWARF debug info for better stack traces
		config.wasm_threads(true);		   // Enable support for wasm threads (atomics)
		config.wasm_simd(true);			   // Enable SIMD instructions
		config.wasm_bulk_memory(true);	   // Enable bulk memory operations
		config.wasm_multi_value(true);	   // Enable multi-value returns
		config.wasm_reference_types(true); // Enable reference types (externref, funcref)
										   // config.cranelift_opt_level(wasmtime::OptLevel::Speed); // Set Cranelift optimization level (0-3)
		// config.strategy(wasmtime::Strategy::Cranelift); // Choose compilation strategy
		// config.profiler(wasmtime::ProfilingStrategy::Jitdump); // Enable JIT profiling
		// config.epoch_interruption(true); // Enable epoch-based interruption
		// config.static_memory_maximum_size(128 * 1024 * 1024); // Set static memory max size
		// config.static_memory_guard_size(2 * 1024 * 1024); // Guard region for static memory
		// config.dynamic_memory_guard_size(2 * 1024 * 1024); // Guard region for dynamic memory

		wasmtime::Engine engine(std::move(config));

		m_store = new wasmtime::Store(engine);

		const std::string cwd = std::filesystem::current_path();

		std::cout << "host/ cwd: " << cwd << std::endl;
		std::cout << "host/ Init WASI\n";
		WasiConfig wasi;
		wasi.inherit_argv();
		wasi.inherit_env();
		wasi.inherit_stdin();
		wasi.inherit_stdout();
		wasi.inherit_stderr();

		WasiVfsMap( path_config, &wasi ); //path_config is passed in from the host plugin

		
		m_store->context().set_wasi(std::move(wasi)).unwrap();

		// Instantiate the module
		wasmtime::Linker linker(engine);
		// try{
		linker.define_wasi().unwrap();
		//}

		// add our custom host API into the wasm mix.
		XP_API::init(linker, m_store);

		// Load the WASM module
		std::vector<uint8_t> wasm_bytes = this->read_file(filename);

		// Compile and instantiate the module
		std::cout << "host/ Compiling user WASM\n";
		auto module = wasmtime::Module::compile(engine, wasm_bytes).unwrap();
		//    if (!module) {
		//        std::cerr << "Failed to create module" << std::endl;
		//        return 1;
		//    }

		// This is for host managed memory pooling which seems to be the exception.

		// // Optionally, configure memory limits or allow memory growth if needed.
		// // By default, WebAssembly memory is growable up to its maximum (if set in the module).
		// // If you want to set custom limits or ensure memory is growable, you can define and import your own Memory:
		// //
		// // Example (not required if your module already defines memory as growable):
		// wasmtime::MemoryType mem_type(2, 2048); // min 1 page, max 100 pages
		// wasmtime::Memory custom_mem = wasmtime::Memory::create(store, mem_type).unwrap();
		// linker.define(store, "env", "memory", custom_mem).unwrap();
		// //
		// // If your module defines memory with a maximum, you can grow it up to that limit.
		// // No extra options are needed unless you want to override the module's memory definition.
		//     auto& memory = custom_mem;

		std::cout << "host/ Instantiate blob.wasm\n";
		m_instance = new wasmtime::Instance(linker.instantiate(m_store, module).unwrap());
		//    if (!instance) {
		//        std::cerr << "Failed to instantiate module" << std::endl;
		//        auto error = instance.err();
		//        std::cerr << "Error message: " << error.message() << std::endl;
		//        return 1;
		//    }
		std::cout << "host/   Success\n";

		std::cout << "host/ Finding WASM memory export..\n";
		// Extract the memory from the instance
		auto memor = m_instance->get(m_store, "memory"); // FIXME: ptr deref?
		if (!memor)
		{
			throw std::runtime_error("host/ Failed to find 'memory' resource");
		}
		m_memory = std::get<Memory>(*memor);

		auto mem_size_pages = m_memory->size(m_store);
		std::cout << "host/         size now (pages): " << mem_size_pages << "\n";
		auto mem_size_kb = (mem_size_pages * 64);
		std::cout << "host/         size now (kilobytes): " << mem_size_kb << " KB\n";

		// find plugin_start,stop,enable,etc
		this->bind_wasm_exports();

		this->set_fuel(m_fuel);

	} // ctor




	~WasmVM()
	{
		delete m_instance;
		delete m_store;
	} // dtor




	void bind_wasm_exports()
	{
		std::cout << "host/ Binding WASM fn exports..\n";

		auto find_and_save_func = [&](std::optional<wasmtime::Func> &save_to, const std::string &fn_name)
		{
			auto maybe_export = m_instance->get(m_store, fn_name.c_str());
			if (maybe_export && std::holds_alternative<Func>(*maybe_export))
			{
				save_to = std::get<Func>(*maybe_export);
				//std::cout << " - " << fn_name << "\n";
			}
			else
			{
				std::cout << "host/  * Warning: " << fn_name << " not found.\n";
			}
		};

		// examine module exports and get handles to fns we need.
		// - malloc / free from wasm module for memory interop.
		find_and_save_func(m_wfn_wasi_start, "_start");
		find_and_save_func(m_wfn_malloc, "malloc");
		find_and_save_func(m_wfn_free, "free");

		// plugin standard set
		find_and_save_func(m_wfn_plugin_start, "plugin_start");
		find_and_save_func(m_wfn_plugin_stop, "plugin_stop");
		find_and_save_func(m_wfn_plugin_enable, "plugin_enable");
		find_and_save_func(m_wfn_plugin_disable, "plugin_disable");
		find_and_save_func(m_wfn_plugin_message, "plugin_message");

		find_and_save_func(m_wfn_plugin_flcb_proxy, "plugin_flcb_proxy");

		// - plugin_update() ?
		// - plugin_flightloop()
		// - plugin_draw() - should we add a bunch specifically for avionics?
	}




	int call_plugin_start( char* outName, char* outSig, char* outDesc )
	{
		// WASI start call.
		{
			this->set_fuel(m_fuel);
			std::cout << "\n> host/ calling wasi _start()\n";
		
			if (m_wfn_wasi_start.has_value()) {
				auto result = m_wfn_wasi_start.value().call(m_store, {}).unwrap();
			} else {
				std::cout << "host/ Error: m_wfn_wasi_start is not set.\n";
			}
		}


		this->set_fuel(m_fuel);

		// plugin start takes 3 char buffers
		
		// Write strings to WebAssembly memory
		int32_t ptr_a = wasm_malloc(256);
		// wasm_strcpy_to( ptr_a, "WASM default name" );
		int32_t ptr_b = wasm_malloc(256);
		// wasm_strcpy_to( ptr_b, "WASM default signature" );
		int32_t ptr_c = wasm_malloc(256);
		// wasm_strcpy_to( ptr_c, "WASM default description" );

		// Call the function with pointers to the strings
		std::vector<wasmtime::Val> args = {
			wasmtime::Val(ptr_a),
			wasmtime::Val(ptr_b),
			wasmtime::Val(ptr_c)};

		std::cout << "\n> host/ calling plugin_start(a,b,c)\n";
		auto result = m_wfn_plugin_start.value().call(m_store, args).unwrap();

		int ret = 0;
		{
			// fflush( stdout );
			// std::cout << "> host/plugin_start returned, result count: " << result.size() << "\n";
			std::cout << "> host/ plugin_start returned: " << result[0].i32() << std::endl;
			ret = result[0].i32();
		}

		if (ret)
		{
			wasm_strcpy_from(outName, ptr_a);
			//std::cout << "  outName: [" << outName << "]\n";
			wasm_strcpy_from(outSig, ptr_b);
			//std::cout << "   outSig: [" << outSig << "]\n";
			wasm_strcpy_from(outDesc, ptr_c);
			//std::cout << "  outDesc: [" << outDesc << "]\n";
		}
		else
		{
			std::cout << "> host/ WASM module refused to start.\n";
		}

		// MUST free resources - these calls count twd fuel usage!
		wasm_free(ptr_a);
		wasm_free(ptr_b);
		wasm_free(ptr_c);


		// DEBUG: Testing use-after-free behaviour. WASM does not clear up old buffers. Data can be leaked.
		#if 0
		{
			char read_back[4096];
			std::cout << "host/  Use after free test. b and c only.\n";
			// Attempt to write to ptr_a after it has been freed
			wasm_strcpy_from(read_back, ptr_b);
			std::cout << "host/    ptr_b readback: [" << read_back << "]\n";
			wasm_strcpy_from(read_back, ptr_c);
			std::cout << "host/    ptr_c readback: [" << read_back << "]\n";
		}
		#endif

		auto lvl = this->check_fuel();

		return ret;
	}




	void call_plugin_stop()
	{
		std::cout << "\n> host/call: plugin_stop\n";
		auto result = m_wfn_plugin_stop.value().call(m_store, {}).unwrap();
		this->check_fuel();
	}




	int call_plugin_enable()
	{
		std::cout << "\n> host/call: plugin_enable\n";

		this->set_fuel(m_fuel * 10);

		auto result = m_wfn_plugin_enable.value().call(m_store, {}).unwrap();


								// --- FLCB fixed call hack
								// --- FLCB fixed call hack
								// --- FLCB fixed call hack
								// --- FLCB fixed call hack
								// --- FLCB fixed call hack
								
								printf( "wasm flcb cbf_ptr: %i\n", xp_api::cb::glob_cbf_ptr );
								auto res_prox = m_wfn_plugin_flcb_proxy.value().call( 
									m_store, 
									{
									wasmtime::Val(xp_api::cb::glob_cbf_ptr)
									}
								).unwrap();
								



		this->check_fuel();

		return 1;
	}




	void call_plugin_disable()
	{

		std::cout << "\n> host/call: plugin_disable\n";
		auto result = m_wfn_plugin_disable.value().call(m_store, {}).unwrap();
		this->check_fuel();
	}




	void call_plugin_message(int64_t inFromWho, int64_t inMessage, int32_t inParam)
	{

		// std::cout << "\n> host/call: plugin_message\n";
		std::vector<wasmtime::Val> args = {
			wasmtime::Val(inFromWho),
			wasmtime::Val(inMessage),
			wasmtime::Val(inParam)};
		auto result = m_wfn_plugin_message.value().call(m_store, args).unwrap();
		// this->check_fuel();
	}




	// fn pair to control WASM compute fuel limits.
	void set_fuel(size_t f)
	{
		m_store->context().set_fuel(f).unwrap();
	}


	size_t check_fuel()
	{
		size_t fuel_level = m_store->context().get_fuel().unwrap();

		{
			uint64_t consumed = m_fuel - fuel_level;
			std::cout << "host/  consumed fuel: " << consumed << "\n";
			double percent = (double(consumed) / double(m_fuel)) * 100.0;
			std::cout << "host/  fuel consumed: " << std::fixed << std::setprecision(1) << percent << "%\n";
		}

		return fuel_level;
	}




private:
	wasmtime::Store *m_store;
	wasmtime::Instance *m_instance;
	std::optional<wasmtime::Memory> m_memory;

	size_t m_fuel = 250'000;

	std::optional<wasmtime::Func> m_wfn_wasi_start;
	std::optional<wasmtime::Func> m_wfn_malloc;
	std::optional<wasmtime::Func> m_wfn_free;

	std::optional<wasmtime::Func> m_wfn_plugin_start;
	std::optional<wasmtime::Func> m_wfn_plugin_stop;
	std::optional<wasmtime::Func> m_wfn_plugin_enable;
	std::optional<wasmtime::Func> m_wfn_plugin_disable;
	std::optional<wasmtime::Func> m_wfn_plugin_message;

	std::optional<wasmtime::Func> m_wfn_plugin_flcb_proxy; //FIXME: might not need this, can we resolve fptr from host?

	//FIXME: we prob want to store this for logging etc?
	// These are passed back to the host caller fn.
	// char outName[256]{};
	// char outSig[256]{};
	// char outDesc[256]{};

	std::vector<uint8_t> read_file(const std::string &filename)
	{

		// Load the WASM module
		std::ifstream wasm_file(filename.c_str(), std::ios::binary);
		if (!wasm_file)
		{
			std::cerr << "host/ Failed to open WASM file [" << filename << "]" << std::endl;
			// return 1;
			throw std::runtime_error("host/ Failed to open WASM file [" + filename + "]");
		}

		std::vector<uint8_t> wasm_bytes((std::istreambuf_iterator<char>(wasm_file)), std::istreambuf_iterator<char>());
		if (wasm_bytes.empty())
		{
			std::cerr << "host/ Failed to read WASM file" << std::endl;
			// return 1;
			throw std::runtime_error("host/ Failed to read WASM file [" + filename + "]");
		}

		return wasm_bytes;
	}

	int32_t wasm_malloc(int32_t size)
	{
		// std::cout << "wasm_malloc called with size: " << size << std::endl;
		if (!m_wfn_malloc.has_value())
		{
			throw std::runtime_error("host/ wasm_malloc: malloc function not found in WASM module");
		}
		std::vector<wasmtime::Val> args = {wasmtime::Val(size)};
		auto result = m_wfn_malloc.value().call(m_store, args).unwrap();
		if (result.empty())
		{
			throw std::runtime_error("host/ wasm_malloc: malloc returned no result");
		}
		// std::cout << "wasm_malloc returned ptr: " << result[0].i32() << std::endl;

		// memory pool is auto growing! nice.
		// auto mem_size_pages = m_memory->size(m_store);
		// std::cout << "        size now (pages): " << mem_size_pages << "\n";
		// auto mem_size_kb = (mem_size_pages * 64);
		// std::cout << "        size now (kilobytes): " << mem_size_kb << " KB\n";

		return result[0].i32();
	}

	int32_t wasm_free(int32_t ptr)
	{
		// std::cout << "wasm_free called with ptr: " << ptr << std::endl;
		if (!m_wfn_free.has_value())
		{
			throw std::runtime_error("host/ wasm_free: free function not found in WASM module");
		}

		// Copy four zero bytes to the memory offset at ptr
		uint32_t zero = 'abcd';
		this->wasm_memcpy_to(ptr, &zero, 4);

		std::vector<wasmtime::Val> args = {wasmtime::Val(ptr)};
		auto result = m_wfn_free.value().call(m_store, args).unwrap();
		if (!result.empty())
		{
			std::cout << "host/ wasm_free returned value: " << result[0].i32() << std::endl;
			return result[0].i32();
		}
		// std::cout << "wasm_free returned 0" << std::endl;
		return 0;
	}

	// copy data into WASM space
	void wasm_memcpy_to(int32_t target_offset, void *data, int32_t size)
	{
		if (!m_memory.has_value())
		{
			throw std::runtime_error("host/ wasm_memcpy_to: WASM memory not available");
		}
		auto mem_data = m_memory->data(*m_store);
		if (target_offset < 0 || target_offset + size > mem_data.size())
		{
			throw std::runtime_error("host/ wasm_memcpy_to: target offset/size out of bounds");
		}
		std::memcpy(mem_data.data() + target_offset, data, size);
	}

	// copy data out of WASM space
	void wasm_memcpy_from(void *data, int32_t target_offset, int32_t size)
	{
		if (!m_memory.has_value())
		{
			throw std::runtime_error("host/ wasm_memcpy_from: WASM memory not available");
		}
		auto mem_data = m_memory->data(*m_store);
		if (target_offset < 0 || target_offset + size > mem_data.size())
		{
			throw std::runtime_error("host/ wasm_memcpy_from: target offset/size out of bounds");
		}
		std::memcpy(data, mem_data.data() + target_offset, size);
	}

	void wasm_strcpy_to(const uint32_t offset, const char *src)
	{
		if (!src)
		{
			throw std::runtime_error("host/ wasm_strcpy_to: src is null");
		}
		size_t len = std::strlen(src) + 1; // include null terminator
		this->wasm_memcpy_to(offset, (void *)src, static_cast<int32_t>(len));
	}

	void wasm_strcpy_from(char *dest, uint32_t offset, int32_t max_len = 4096)
	{
		if (!m_memory.has_value())
		{
			throw std::runtime_error("host/ wasm_strcpy_from: WASM memory not available");
		}
		auto mem_data = m_memory->data(*m_store);
		if (offset < 0 || offset >= mem_data.size())
		{
			throw std::runtime_error("host/ wasm_strcpy_from: offset out of bounds");
		}

		int32_t i = 0;
		// for (; i < max_len - 1 && (offset + i) < mem_data.size(); ++i) {
		for (;;)
		{
			if (i > mem_data.size())
			{
				// std::cout << " * end of memory break\n";
				break;
			}

			dest[i] = mem_data[offset + i];
			if (dest[i] == '\0')
			{
				// std::cout << " * hit null byte break\n";
				break;
			}

			++i;
		}
		dest[i] = '\0';
	}
};


#endif