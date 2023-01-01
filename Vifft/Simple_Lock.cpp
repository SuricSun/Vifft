#include"Simple_Synchronization.h"

Suancai::Util::Simple_Mutex::Simple_Mutex() {

}

Suancai::Util::Simple_Mutex::Simple_Mutex(void* p_initial_locked_data) {

    this->p_locked_data = p_initial_locked_data;
}

bool Suancai::Util::Simple_Mutex::is_locked() {

    if (InterlockedCompareExchange(addr(this->cur_thread_id), 0, 0) != 0) {
        //正在被其他线程使用或还没有get lock
        return true;
    }
    return false;
}

bool Suancai::Util::Simple_Mutex::is_owner() {

    u32 tid = GetCurrentThreadId();
    if (InterlockedCompareExchange(addr(this->cur_thread_id), tid, tid) != tid) {
        //正在被其他线程使用或还没有get lock
        return false;
    }
    return true;
}

bool Suancai::Util::Simple_Mutex::try_get() {

    if (this->is_owner()) {
        //有锁直接返回
        return true;
    }
    //否则锁的状态只有0或者被其他线程拥有
    //尝试获取
    u32 tid = GetCurrentThreadId();
    if (InterlockedCompareExchange(addr(this->cur_thread_id), tid, 0) != 0) {
        //一定被其他线程获取
        return false;
    }
    //获取成功
    InterlockedExchange(addr(this->cur_thread_id), tid);
    return true;
}

bool Suancai::Util::Simple_Mutex::release() {

    if (this->is_owner() == false) {
        //没锁直接返回
        return false;
    }
    //取消锁
    InterlockedExchange(addr(this->cur_thread_id), 0);
    return true;
}

bool Suancai::Util::Simple_Mutex::try_get_With_locked_data(void** pp_data) {

    if (this->try_get() == false) {
        return false;
    }
    if (pp_data != nullptr) {
        deref(pp_data) = this->p_locked_data;
    }
    return true;
}

bool Suancai::Util::Simple_Mutex::release_With_locked_data(void* p_data_to_lock) {

    if (this->is_owner() == false) {
        return false;
    }
    this->p_locked_data = p_data_to_lock;
    this->release();
    return true;
}

Suancai::Util::Simple_Mutex::~Simple_Mutex() {
}
