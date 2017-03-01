#include "libsk.h"
#include "bus_router.h"
#include "bus_config.h"
#include "shm/detail/shm_segment.h"
#include "bus/detail/channel_mgr.h"
#include "bus/detail/channel.h"
#include "common/lock_guard.h"

class busd;
typedef sk::server<bus_config, busd> server_type;
class busd : public server_type {
protected:
    virtual int on_init() {
        int ret = 0;
        const bus_config& cfg = config();
        const sk::server_context& ctx = context();

        ret = router_.init(cfg, ctx.resume_mode);
        if (ret != 0) return ret;

        return 0;
    }

    virtual int on_fini() {
        router_.fini();
        return 0;
    }

    virtual int on_stop() {
        return 0;
    }

    virtual int on_run() {
        return router_.run();
    }

    virtual int on_reload() {
        router_.reload(config());
        return 0;
    }

    virtual void on_msg(int src_busid, const void *buf, size_t len) {
        sk_assert(0);
    }

private:
    bus_router router_;
};


int main(int argc, const char **argv) {
    server_type& s = server_type::get();
    int ret = 0;

    ret = s.init(argc, argv);
    if (ret != 0) return ret;

    s.run();

    ret = s.fini();
    if (ret != 0) return ret;

    return 0;
}
