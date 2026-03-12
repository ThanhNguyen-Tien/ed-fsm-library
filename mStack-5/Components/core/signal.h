#ifndef CORE_SIGNAL_H
#define CORE_SIGNAL_H

#include <core/event.h>
#include <core/mem-pool.h>

namespace core
{
    /**
     * @brief Internal node for SignalMany connections.
     * Aligned to 4-bytes for single-cycle access on ARM Cortex-M/R.
     */
    struct alignas(4) SignalNode {
        void* event;      
        SignalNode* next; 
    };

    extern MemPool<SignalNode> defaultSignalPool;

    // --- SignalOne (SPSC, No Pool) ---
    class EmptySignalOne {
    public:
        void connect(EmptyEvent *event) { this->event_ = event; }
        void disconnect() { event_ = nullptr; }
        inline void emit() { if (event_) event_->post(); }
    private:
        EmptyEvent *event_ = nullptr;
    };

    template <typename EV, typename E>
    class SignalOne {
    public:
        void connect(EV *event) { this->event_ = event; }
        void disconnect() { event_ = nullptr; }
        inline void emit(E e) { if (event_) event_->post(e); }
    private:
        EV *event_ = nullptr;
    };

    // --- BaseSignalMany (SPSC Management, Thread-safe Emit) ---
    template <class E>
    class BaseSignalMany {
    public:
        BaseSignalMany(MemPool<SignalNode>& pool = defaultSignalPool) : pool_(pool) {}

        void connect(E *event) {
            // SPSC: Only from Main loop. Use Acquire to see latest list state.
            SignalNode* it = __atomic_load_n(&nodes_, __ATOMIC_ACQUIRE);
            while (it != nullptr) {
                if (it->event == static_cast<void*>(event)) return;
                it = it->next;
            }

            void* mem = pool_.Alloc();
            if (!mem) return;

            SignalNode* newNode = static_cast<SignalNode*>(mem);
            newNode->event = static_cast<void*>(event);
            
            // Release: Payload (event pointer) must be written before making node visible
            newNode->next = __atomic_load_n(&nodes_, __ATOMIC_RELAXED);
            __atomic_store_n(&nodes_, newNode, __ATOMIC_RELEASE);
        }

        void disconnect(E *event) {
            SignalNode* pre = nullptr;
            SignalNode* it = __atomic_load_n(&nodes_, __ATOMIC_ACQUIRE);
            
            while (it != nullptr) {
                if (it->event == static_cast<void*>(event)) {
                    if (pre == nullptr)
                        __atomic_store_n(&nodes_, it->next, __ATOMIC_RELEASE);
                    else
                        pre->next = it->next;
                    
                    pool_.Free(it);
                    return;
                }
                pre = it;
                it = it->next;
            }
        }

    protected:
        MemPool<SignalNode>& pool_;
        SignalNode* nodes_ = nullptr; // Atomic ops handle visibility/ordering
    };

    // --- Specific SignalMany Classes ---
    class EmptySignalMany : public BaseSignalMany<EmptyEvent> {
    public:
        using BaseSignalMany<EmptyEvent>::BaseSignalMany;
        inline void emit() {
            SignalNode* it = __atomic_load_n(&nodes_, __ATOMIC_ACQUIRE);
            while (it) {
                static_cast<EmptyEvent*>(it->event)->post();
                it = it->next;
            }
        }
    };

    template <typename EV, typename E>
    class SignalMany : public BaseSignalMany<EV> {
    public:
        using BaseSignalMany<EV>::BaseSignalMany;
        inline void emit(E e) {
            SignalNode* it = __atomic_load_n(&this->nodes_, __ATOMIC_ACQUIRE);
            while (it) {
                static_cast<EV*>(it->event)->post(e);
                it = it->next;
            }
        }
    };
}

// --- Macros ---
#define _M_GET_MANY_ARG(_1, _2, _3, NAME, ...) NAME
#define M_SIGNAL_MANY(...) _M_GET_MANY_ARG(__VA_ARGS__, _M_MANY_3, _M_MANY_2, _M_MANY_1)(__VA_ARGS__)

#define _M_MANY_1(name) \
public: core::EmptySignalMany name##Signal;

#define _M_MANY_2(name, type) \
public: core::SignalMany<core::FixedEvent<type>, type> name##Signal;

#define _M_MANY_3(name, type, pool) \
public: core::SignalMany<core::FixedEvent<type>, type> name##Signal{pool};

#define M_SIGNAL_MANY_POOL(name, pool) \
public: core::EmptySignalMany name##Signal{pool};

#define _M_GET_ONE_ARG(_1, _2, NAME, ...) NAME
#define M_SIGNAL(...) _M_GET_ONE_ARG(__VA_ARGS__, _M_FIXED_SIGNAL_ONE, _M_SIGNAL_ONE)(__VA_ARGS__)

#define _M_SIGNAL_ONE(name) \
public: core::EmptySignalOne name##Signal;

#define _M_FIXED_SIGNAL_ONE(name, type) \
public: core::SignalOne<core::FixedEvent<type>, type> name##Signal;

#endif
