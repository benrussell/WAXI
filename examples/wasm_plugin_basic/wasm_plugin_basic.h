
// Bare bones test harness. Mandatory sigs.



extern "C" {
    
    int plugin_start(char* outName, char* outSig, char* outDesc);
    void plugin_stop();

    int plugin_enable();
    void plugin_disable();

    void plugin_message( int64_t inFromWho, int64_t inMessage, int32_t inParam );


    //it may be possible to replace this with fn resolution at the host side.
    void plugin_flcb_proxy( int32_t fptr );


} //extern


int main(void); //export not required.

