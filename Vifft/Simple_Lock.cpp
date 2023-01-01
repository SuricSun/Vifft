#include"Simple_Synchronization.h"

Suancai::Util::Simple_Mutex::Simple_Mutex() {

}

Suancai::Util::Simple_Mutex::Simple_Mutex(void* p_initial_locked_data) {

    this->p_locked_data = p_initial_locked_data;
}

bool Suancai::Util::Simple_Mutex::is_locked() {

    if (InterlockedCompareExchange(addr(this->cur_thread_id), 0, 0) != 0) {
        //���ڱ������߳�ʹ�û�û��get lock
        return true;
    }
    return false;
}

bool Suancai::Util::Simple_Mutex::is_owner() {

    u32 tid = GetCurrentThreadId();
    if (InterlockedCompareExchange(addr(this->cur_thread_id), tid, tid) != tid) {
        //���ڱ������߳�ʹ�û�û��get lock
        return false;
    }
    return true;
}

bool Suancai::Util::Simple_Mutex::try_get() {

    if (this->is_owner()) {
        //����ֱ�ӷ���
        return true;
    }
    //��������״ֻ̬��0���߱������߳�ӵ��
    //���Ի�ȡ
    u32 tid = GetCurrentThreadId();
    if (InterlockedCompareExchange(addr(this->cur_thread_id), tid, 0) != 0) {
        //һ���������̻߳�ȡ
        return false;
    }
    //��ȡ�ɹ�
    InterlockedExchange(addr(this->cur_thread_id), tid);
    return true;
}

bool Suancai::Util::Simple_Mutex::release() {

    if (this->is_owner() == false) {
        //û��ֱ�ӷ���
        return false;
    }
    //ȡ����
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
