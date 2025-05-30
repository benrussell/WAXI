#include <wasmtime.hh>
#include <iostream>
#include <vector>
#include <optional>
#include <cstdint>

using namespace wasmtime;

#include "WasmVM.h"

//#include <XPLMDisplay.h> //FIXME: SDK coupled ??

/*──────────────────────────── wasm_cb_wrap ───────────────────────────────────*/
class wasm_cb_wrap {
	wasmtime::Store* m_store;		// long-lived       (never Store::Context)
	wasmtime::Func   m_func;		// guest callback
	int32_t          m_refcon;		// guest-side ptr

public:
	wasm_cb_wrap(wasmtime::Store* store,
	             wasmtime::Func   func,
	             int32_t          wasm_refcon)
	  : m_store(store), m_func(func), m_refcon(wasm_refcon)
	{
		//std::cout << "waxi/ wasm_cb_wrap ctor store=" << store << '\n';

		XPLMCreateFlightLoop_t fl{};
		fl.structSize   = sizeof fl;
		fl.callbackFunc = &wasm_cb_wrap::xplane_flcb_handler;
		fl.refcon       = this;
		fl.phase        = xplm_FlightLoop_Phase_BeforeFlightModel; //FIXME: before/afer param

		XPLMFlightLoopID id = XPLMCreateFlightLoop(&fl);
		XPLMScheduleFlightLoop(id, 1.0f, /*relative*/1); //FIXME: relative cb sched param
	}

	float call_wasm_fptr(float dt,float dt_fl,int counter)
	{
		auto ctx = m_store->context();				// fresh borrow every call
		std::vector<wasmtime::Val> args = {
			dt, dt_fl, counter, int32_t(m_refcon)
		};
        auto result = m_func.call(ctx, args).unwrap();
        float ret = result[0].f32();
		return ret;
	}

	static float xplane_flcb_handler(float dt,float dt_fl,int counter,void* ref)
	{
		return static_cast<wasm_cb_wrap*>(ref)
		       ->call_wasm_fptr(dt, dt_fl, counter);
	}
};






/*──────────────────────────── wasm_draw_cb_wrap ───────────────────────────────────*/
class wasm_draw_cb_wrap {
	wasmtime::Store* m_store;		// long-lived       (never Store::Context)
	wasmtime::Func   m_func;		// guest callback
	int32_t          m_refcon;		// guest-side ptr

public:
	wasm_draw_cb_wrap(wasmtime::Store* store,
	             wasmtime::Func   func,
	             int32_t          wasm_refcon)
	  : m_store(store), m_func(func), m_refcon(wasm_refcon)
	{
		//std::cout << "waxi/ wasm_draw_cb_wrap ctor store=" << store << '\n';

		XPLMRegisterDrawCallback(
			&wasm_draw_cb_wrap::xplane_drawcb_handler, 
			xplm_Phase_Window, //FIXME: phase param
			0, //FIXME: before/after param
			this
			);

	}


	~wasm_draw_cb_wrap(){
		XPLMUnregisterDrawCallback(
			&wasm_draw_cb_wrap::xplane_drawcb_handler,
			xplm_Phase_Window, //FIXME: member var
			0, //FIXME: before/after - member var
			this
		);
	}


	int call_wasm_fptr(
		XPLMDrawingPhase phase, 
		int is_before
	){
		// printf("waxi/ cb/ drawcb_handler->call_wasm_fptr() phase: %i before: %i m_refcon: %p\n",
		// 	phase,
		// 	is_before,
		// 	m_refcon
		// );

		auto ctx = m_store->context();				// fresh borrow every call
		std::vector<wasmtime::Val> args = {
			phase, is_before, int32_t(m_refcon)
		};
        auto result = m_func.call(ctx, args).unwrap();
        int ret = result[0].i32();
		return ret;
	}


	static int xplane_drawcb_handler(
		XPLMDrawingPhase phase, 
		int is_before, 
		void* ref
	){
		//printf("waxi/ xplane_drawcb_handler..\n");
		return static_cast<wasm_draw_cb_wrap*>(ref)
		       ->call_wasm_fptr(phase, is_before);
	}
};






/*─────────────────────────── xp_api::cb host import ─────────────────────────*/
namespace xp_api {
class cb {
public:
	static std::vector<wasm_cb_wrap*> callbacks;

	static int64_t reg(wasmtime::Caller caller, int32_t idx, int32_t wasm_refcon)
	{
		// std::cout << ">> api/ cb_reg(" << idx << ", " << wasm_refcon << ")\n";


		//FIXME: this block of code casts via a void* 'cause I was tired of fighting the compiler.
		// it should be improved. but it "works for now". (2025-05-28, br)
		auto store_data_any = caller.context().get_data();
		auto raw_store_data_ptr = store_data_any.has_value() && store_data_any.type() == typeid(void*)
								  ? std::any_cast<void*>(store_data_any)
								  : nullptr;
        if (!raw_store_data_ptr) {
            throw std::runtime_error("Failed to unpack store data into void*");
        }

        auto* store_ptr = (wasmtime::Store*)raw_store_data_ptr;
        //printf("xp_api::cb::reg/ store_ptr: %p\n", store_ptr);
        // printf("xp_api::cb::reg/ wasm_refcon: %p\n", wasm_refcon);


        auto tbl_any = caller.get_export("__indirect_function_table");
		auto* tbl    = tbl_any ? std::get_if<wasmtime::Table>(&*tbl_any) : nullptr;
		if (!tbl) { std::cerr << "table not found\n"; return -1; }

		auto val_opt  = tbl->get(caller, idx);
		auto func_opt = val_opt ? val_opt->funcref() : std::nullopt;
		if (!func_opt) throw std::runtime_error("index not a funcref");


		callbacks.emplace_back(new wasm_cb_wrap(store_ptr, *func_opt, wasm_refcon));
		return callbacks.size();
	}

	static void unreg(wasmtime::Caller, int64_t) {
		/*TODO*/
		printf("xp_api::cb::unreg NOOP\n");
		exit(1);
	}





	static std::vector<wasm_draw_cb_wrap*> draw_callbacks;

	//idx: the wasm fn ptr that we callback
	//wasm_refcon: user data ptr
	static int64_t reg_draw(wasmtime::Caller caller, int32_t idx, int32_t wasm_refcon)
	{
		//std::cout << ">> api/ cb_reg_draw(" << idx << ", " << wasm_refcon << ")\n";

		//FIXME: this block of code casts via a void* 'cause I was tired of fighting the compiler.
		// it should be improved. but it "works for now". (2025-05-28, br)
		auto store_data_any = caller.context().get_data();
		auto raw_store_data_ptr = store_data_any.has_value() && store_data_any.type() == typeid(void*)
								  ? std::any_cast<void*>(store_data_any)
								  : nullptr;
        if (!raw_store_data_ptr) {
            throw std::runtime_error("Failed to unpack store data into void*");
        }

        auto* store_ptr = (wasmtime::Store*)raw_store_data_ptr;
        //printf("xp_api::cb::reg/ store_ptr: %p\n", store_ptr);
        // printf("xp_api::cb::reg/ wasm_refcon: %p\n", wasm_refcon);


        auto tbl_any = caller.get_export("__indirect_function_table");
		auto* tbl    = tbl_any ? std::get_if<wasmtime::Table>(&*tbl_any) : nullptr;
		if (!tbl) { std::cerr << "table not found\n"; return -1; }

		auto val_opt  = tbl->get(caller, idx);
		auto func_opt = val_opt ? val_opt->funcref() : std::nullopt;
		if (!func_opt) throw std::runtime_error("index not a funcref");


		draw_callbacks.emplace_back(new wasm_draw_cb_wrap(store_ptr, *func_opt, wasm_refcon));
		return callbacks.size();
	}


	static void unreg_draw(wasmtime::Caller, int64_t) {
		/*TODO*/
		printf("xp_api::cb::unreg NOOP\n");
		exit(1);
	}




};



std::vector<wasm_cb_wrap*> cb::callbacks;
std::vector<wasm_draw_cb_wrap*> cb::draw_callbacks;


} // namespace xp_api

