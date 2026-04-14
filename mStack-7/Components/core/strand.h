#ifndef CORE_STRAND_H
#define CORE_STRAND_H

#include <core/event.h>
#include <core/event-queue.h>
#include <core/engine.h>
#include <core/queue/spsc-queue.h>
#include <core/timer.h>

namespace core
{
	template <uint16_t N>
    class Strand : public Component
    {
    public:
        Strand(SpscQueue<uint8_t, N> &queue) : queue_(queue) {}

        bool post(EmptyEvent *event, WordEvent *finished = nullptr)
        {
            // 1(TYPE) + 2(FINISHED_ID) + 2(EVENT_ID) + 1(LEN=0) = 6 bytes
            if (queue_.freeSpace() < 6) return false;

            bool sts = queue_.push(finished ? CALLBACK : VOID);

            // Push Finished ID (16-bit)
            uint16_t f_id = finished ? finished->getId() : 0xFFFF;
            sts &= queue_.push(static_cast<uint8_t>(f_id & 0xFF));
            sts &= queue_.push(static_cast<uint8_t>((f_id >> 8) & 0xFF));

            // Push Event ID (16-bit)
            uint16_t e_id = event->getId();
            sts &= queue_.push(static_cast<uint8_t>(e_id & 0xFF));
            sts &= queue_.push(static_cast<uint8_t>((e_id >> 8) & 0xFF));

            sts &= queue_.push(0); // Length = 0

            next_();
            return sts;
        }

        template<typename E>
        bool post(FixedEvent<E> *event, const E &data, WordEvent *finished = nullptr)
        {
            static_assert(sizeof(E) <= 255, "Payload too large");
            uint8_t len = static_cast<uint8_t>(sizeof(E));
            if (queue_.freeSpace() < static_cast<uint32_t>(6 + len)) return false;

            bool sts = queue_.push(finished ? CALLBACK : VOID);

            // Push Finished ID
            uint16_t f_id = finished ? finished->getId() : 0xFFFF;
            sts &= queue_.push(static_cast<uint8_t>(f_id & 0xFF));
            sts &= queue_.push(static_cast<uint8_t>((f_id >> 8) & 0xFF));

            // Push Event ID
            uint16_t e_id = event->getId();
            sts &= queue_.push(static_cast<uint8_t>(e_id & 0xFF));
            sts &= queue_.push(static_cast<uint8_t>((e_id >> 8) & 0xFF));

            sts &= queue_.push(len);

            // Push Payload
            const uint8_t* pData = static_cast<const uint8_t*>(static_cast<const void*>(&data));
            for (uint8_t i = 0; i < len; i++) queue_.push(pData[i]);

            next_();
            return sts;
        }

		void done(uint32_t result = 0) {
			__atomic_clear(&busy_, __ATOMIC_RELEASE);
			if (finished_ != nullptr) {
				finished_->post(result);
				finished_ = nullptr;
			}
			next_();
		}

		bool delay(uint32_t ms) {
			if (queue_.freeSpace() < 5) return false;
			bool sts = queue_.push(DELAY);
			sts &= queue_.push(static_cast<uint8_t>((ms >> 24) & 0xFF));
			sts &= queue_.push(static_cast<uint8_t>((ms >> 16) & 0xFF));
			sts &= queue_.push(static_cast<uint8_t>((ms >> 8) & 0xFF));
			sts &= queue_.push(static_cast<uint8_t>(ms & 0xFF));
			next_();
			return sts;
		}

	private:
		enum EventType : uint8_t { VOID = 1, CALLBACK, DELAY };

		void next_()
		{
			if (__atomic_test_and_set(&busy_, __ATOMIC_ACQUIRE)) return;
			if (queue_.isEmpty())
			{
				__atomic_clear(&busy_, __ATOMIC_RELEASE);
				return;
			}
			if (!executeEvent_.post()) __atomic_clear(&busy_, __ATOMIC_RELEASE);
		}

		void execute_()
		{
			if (queue_.isEmpty())
			{
				__atomic_clear(&busy_, __ATOMIC_RELEASE);
				return;
			}

			uint8_t type { 0U };
			bool sts = queue_.pop(type);

			if (type == DELAY)
			{
				uint32_t ms { 0U };
				uint8_t val { 0U };
				sts &= queue_.pop(val);
				ms|= (static_cast<uint32_t>(val) << 24);
				sts &= queue_.pop(type);
				ms |= (static_cast<uint32_t>(val) << 16);
				sts &= queue_.pop(type);
				ms |= (static_cast<uint32_t>(val) << 8);
				sts &= queue_.pop(type);
				ms |= static_cast<uint32_t>(val);
				timer_.start(ms, 1);
				if(!sts)
				{
					__atomic_clear(&busy_, __ATOMIC_RELEASE);
//					telemetry::log(TelemetryType::STRAND_QUEUE_ERROR, 0);
				}
			}
			else
			{
				// Pop Finished ID (16-bit)
				uint8_t val { 0U };
				uint16_t f_id { 0U};
				sts &= queue_.pop(val);
				f_id = val;
				sts &= queue_.pop(val);
				f_id |= (static_cast<uint16_t>(val) << 8);

				if (type == CALLBACK)
				{
					if (f_id < events_.poolSize_)
					{
						finished_ = (WordEvent*) events_.events_[f_id];
					}
					else
					{
						finished_ = nullptr;
					}
				}

				// Pop Event ID (16-bit)
				uint16_t ev_id { 0U };
				sts &= queue_.pop(val);
				ev_id = val;
				sts &= queue_.pop(val);
				ev_id |= (static_cast<uint16_t>(val) << 8);

				// Pop Length and Payload
				uint8_t len { 0U };
				sts &= queue_.pop(len);

				Event *e = events_.events_[ev_id];
				if (e)
				{
					if (len > 0)
					{
						uint8_t stackBuf[len] alignas(4);
						for(uint8_t i = 0; i < len; i++)
						{
							sts = queue_.pop(stackBuf[i]);
						}
						if (!sts)
						{
							__atomic_clear(&busy_, __ATOMIC_RELEASE);
//							telemetry::log(TelemetryType::STRAND_QUEUE_ERROR, 0);
						}
						e->execute(stackBuf);
					}
					else
					{
						e->execute(nullptr);
					}
				}
			}
		}

        void timeout_()
        {
            __atomic_clear(&this->busy_, __ATOMIC_RELEASE);
            this->next_();
        }

	private:
		SpscQueue<uint8_t, N> &queue_;
		WordEvent *finished_ = nullptr;
		EventQueue<CORE_NUM_PRIORITIES> &events_ = Engine::instance().events();
		EmptyEvent executeEvent_ = EmptyEvent(this, static_cast<EmptyEvent::Handler>(&Strand::execute_));
		Timer timer_ = Timer(this, static_cast<Timer::Handler>(&Strand::timeout_));
		volatile bool busy_ = false;
	};
}

#define M_STRAND(name, size)                        \
private:                                            \
	core::SpscQueue<uint8_t, size> name##Queue_; \
public:                                             \
	core::Strand<size> name##Strand{name##Queue_};

#endif // STRAND_H
