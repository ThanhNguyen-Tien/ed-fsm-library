#ifndef CORE_SIGNAL_H
#define CORE_SIGNAL_H
#include <core/event.h>
#include <core/base.h>
#include <core/mem-pool.h>

namespace core
{
	struct SignalConnection {
		void* event;
		SignalConnection* next;
	};
	extern MemPool<SignalConnection> defaultSignalPool;

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

    template <class E>
    class BaseSignalMany {
    protected:
        struct Connection {
            E *event;
            Connection *next;
        };

    public:
        BaseSignalMany(MemPool<SignalConnection>& pool = defaultSignalPool)
            : pool_(pool) {}

        void connect(E *event) {
            // SPSC: Only call from Main loop, check duplicate
            for (Connection *it = (Connection*)connections_; it != nullptr; it = it->next) {
                if (it->event == event) return;
            }

            void* mem = pool_.Alloc(); // Dùng pool được chỉ định
            if (!mem) {
                Telemetry::log(TelemetryType::SIGNAL_POOL_FULL);
                return;
            }

            Connection *con = static_cast<Connection*>(mem);
            con->event = event;
            con->next = (Connection*)connections_;
            // Release to ensure the new connection is visible before updating the head pointer
            __atomic_store_n(&connections_, con, __ATOMIC_RELEASE);
        }

        void disconnect(E *event) {
            Connection *pre = nullptr;
            for (Connection *it = (Connection*)connections_; it != nullptr; it = it->next) {
                if (it->event == event) {
                    if (pre == nullptr)
                        __atomic_store_n(&connections_, it->next, __ATOMIC_RELEASE);
                    else
                        pre->next = it->next;

                    pool_.Free(it);
                    return;
                }
                pre = it;
            }
        }

    protected:
        MemPool<SignalConnection>& pool_;
        volatile Connection *connections_ = nullptr;
    };

    class EmptySignalMany : public BaseSignalMany<EmptyEvent> {
    public:
        using BaseSignalMany<EmptyEvent>::BaseSignalMany;
        inline void emit() {
            Connection *it = (Connection*)__atomic_load_n(&connections_, __ATOMIC_ACQUIRE);
            for (; it != nullptr; it = it->next) {
                if (it->event) it->event->post();
            }
        }
    };

    template <typename EV, typename E>
    class SignalMany : public BaseSignalMany<EV> {
    public:
        using BaseSignalMany<EV>::BaseSignalMany;
        inline void emit(E e) {
            auto it = (typename BaseSignalMany<EV>::Connection*)__atomic_load_n(&this->connections_, __ATOMIC_ACQUIRE);
            for (; it != nullptr; it = it->next) {
                if (it->event) it->event->post(e);
            }
        }
    };
}

// --- MACRO FUNCTIONS --
#define M_SIGNAL(...) _M_MACRO_2(__VA_ARGS__, _M_FIXED_SIGNAL_ONE, _M_SIGNAL_ONE)(__VA_ARGS__)
#define M_SIGNAL_MANY(...) _M_MACRO_3(__VA_ARGS__, _M_MANY_3, _M_MANY_2, _M_MANY_1)(__VA_ARGS__)

#define _M_SIGNAL_ONE(name) \
public: core::EmptySignalOne name##Signal;

#define _M_FIXED_SIGNAL_ONE(name, type) \
public: core::SignalOne<core::FixedEvent<type>, type> name##Signal;

#define _M_MANY_1(name) \
public: core::EmptySignalMany name##Signal;

#define _M_MANY_2(name, type) \
public: core::SignalMany<core::FixedEvent<type>, type> name##Signal;

#define _M_MANY_3(name, type, pool) \
public: core::SignalMany<core::FixedEvent<type>, type> name##Signal{pool};

#define M_SIGNAL_MANY_POOL(name, pool) \
public: core::EmptySignalMany name##Signal{pool};

#endif // SIGNAL_H

