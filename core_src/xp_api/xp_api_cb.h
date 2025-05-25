#include <wasmtime.hh>
#include <iostream>
#include <vector>
#include <optional>
#include <cstdint>

using namespace wasmtime;

#include "WasmVM.h"

/*──────────────────────────── wasm_cb_wrap ───────────────────────────────────*/
class wasm_cb_wrap {
	wasmtime::Store* m_store;		// long-lived       (never Store::Context)
	wasmtime::Func   m_func;		// guest callback
	int32_t          m_refcon;		// guest-side ptr

public:
	wasm_cb_wrap(wasmtime::Store* store,
	             wasmtime::Func   func,
	             int32_t          refcon)
	  : m_store(store), m_func(func), m_refcon(refcon)
	{
		std::cout << "> wasm_cb_wrap ctor store=" << store << '\n';

		XPLMCreateFlightLoop_t fl{};
		fl.structSize   = sizeof fl;
		fl.callbackFunc = &wasm_cb_wrap::xplane_flcb_handler;
		fl.refcon       = this;
		fl.phase        = xplm_FlightLoop_Phase_BeforeFlightModel;

		XPLMFlightLoopID id = XPLMCreateFlightLoop(&fl);
		XPLMScheduleFlightLoop(id, 1.0f, /*relative*/1);
	}

	float call_wasm_ftpr(float dt,float dt_fl,int counter)
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
		       ->call_wasm_ftpr(dt, dt_fl, counter);
	}
};

/*─────────────────────────── xp_api::cb host import ─────────────────────────*/
namespace xp_api {
class cb {
public:
	static std::vector<wasm_cb_wrap*> callbacks;

	static int64_t reg(wasmtime::Caller caller, int32_t idx)
	{
		std::cout << ">> api/ cb_reg(" << idx << ")\n";

		auto store_data_any = caller.context().get_data();
		auto raw_store_data_ptr = store_data_any.has_value() && store_data_any.type() == typeid(void*)
								  ? std::any_cast<void*>(store_data_any)
								  : nullptr;
        if (!raw_store_data_ptr) {
            throw std::runtime_error("Failed to unpack store data into void*");
        }

        auto* store_ptr = (wasmtime::Store*)raw_store_data_ptr;
        printf("xp_api::cb::reg/ store_ptr: %p\n", store_ptr);


        auto tbl_any = caller.get_export("__indirect_function_table");
		auto* tbl    = tbl_any ? std::get_if<wasmtime::Table>(&*tbl_any) : nullptr;
		if (!tbl) { std::cerr << "table not found\n"; return -1; }

		auto val_opt  = tbl->get(caller, idx);
		auto func_opt = val_opt ? val_opt->funcref() : std::nullopt;
		if (!func_opt) throw std::runtime_error("index not a funcref");


		callbacks.emplace_back(new wasm_cb_wrap(store_ptr, *func_opt, 0xABCD));
		return callbacks.size();
	}

	static void unreg(wasmtime::Caller, int64_t) {/*TODO*/}
};
std::vector<wasm_cb_wrap*> cb::callbacks;
} // namespace xp_api

