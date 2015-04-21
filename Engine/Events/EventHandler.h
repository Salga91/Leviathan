#ifndef LEVIATHAN_EVENTHANDLER
#define LEVIATHAN_EVENTHANDLER
// ------------------------------------ //
#ifndef LEVIATHAN_DEFINE
#include "Define.h"
#endif
// ------------------------------------ //
// ---- includes ---- //
#include "Event.h"
#include "CallableObject.h"

namespace Leviathan{

	struct RegisteredCallback{
		RegisteredCallback(CallableObject* receiver, EVENT_TYPE type) : Receiver(receiver), Type(type){
		}
		~RegisteredCallback(){
		}

		CallableObject* Receiver;
		EVENT_TYPE Type;
	};
	// callbacks are split to normal and generic, should probably improve performance //
	struct GenericRegisteredCallback{
		GenericRegisteredCallback(CallableObject* receiver, const wstring &type) : Receiver(receiver), Type(type){
		}
		~GenericRegisteredCallback(){
		}

		CallableObject* Receiver;
		wstring Type;
	};


	class EventHandler : public EngineComponent, public ThreadSafe{
	public:
		DLLEXPORT EventHandler();
		DLLEXPORT ~EventHandler();

		DLLEXPORT bool Init();
		DLLEXPORT void Release();

        //! \todo Make this use references instead
		DLLEXPORT void CallEvent(Event* pEvent);
        //! \todo Allow these events to also delete themselves
		DLLEXPORT void CallEvent(GenericEvent* pEvent);

		DLLEXPORT bool RegisterForEvent(CallableObject* toregister, EVENT_TYPE totype);
		DLLEXPORT bool RegisterForEvent(CallableObject* toregister, const wstring &genericname);
		DLLEXPORT void Unregister(CallableObject* caller, EVENT_TYPE type, bool all = false);
		DLLEXPORT void Unregister(CallableObject* caller, const wstring &genericname, bool all = false);

		DLLEXPORT static EventHandler* Get();

		DLLEXPORT void CallEventGenericProxy(GenericEvent* genericevent);

	private:
		std::vector<unique_ptr<RegisteredCallback>> EventListeners;
		std::vector<GenericRegisteredCallback*> GenericEventListeners;

		static EventHandler* main;
	};

}
#endif
