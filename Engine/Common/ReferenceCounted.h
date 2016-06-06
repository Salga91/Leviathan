#pragma once
// ------------------------------------ //
#include "Define.h"
// ------------------------------------ //
#include <cstdint>
#include <atomic>
#ifdef USING_BOOST
#include <boost/intrusive_ptr.hpp>
#endif // USING_BOOST


namespace Leviathan{

	// macro for adding proxies to hopefully work with scripts //
#define REFERENCECOUNTED_ADD_PROXIESFORANGELSCRIPT_DEFINITIONS(classname) void AddRefProxy(){ \
        this->AddRef(); }; void ReleaseProxy(){ this->Release(); };

    //! Reference counted object which will be deleted when all references are gone
    //! \note Pointers can be used with ReferenceCounted::pointer ptr = new Object();
    //! \todo Make sure that all functions using intrusive pointers use the MakeIntrusive function
	class ReferenceCounted{
	public:

#ifdef USING_BOOST
        typedef boost::intrusive_ptr<ReferenceCounted> pointer;
#endif // USING_BOOST

		DLLEXPORT inline ReferenceCounted() : RefCount(1){}
		DLLEXPORT virtual ~ReferenceCounted(){}

		DLLEXPORT FORCE_INLINE void AddRef(){

            intrusive_ptr_add_ref(this);
		}
        
		//! removes a reference and deletes the object if reference count reaches zero
		DLLEXPORT FORCE_INLINE void Release(){

            intrusive_ptr_release(this);
        }

#ifdef USING_BOOST
        //! \brief Creates an intrusive_ptr from raw pointer
        template<class ActualType>
        DLLEXPORT static boost::intrusive_ptr<ActualType> MakeShared(ActualType* ptr){

            if(!ptr)
                return nullptr;

            boost::intrusive_ptr<ActualType> newptr(ptr);
            ptr->Release();

            return newptr;
        }
#endif // USING_BOOST

        //! \brief Returns the reference count
        //! \todo Make sure that the right memory order is used
        DLLEXPORT int32_t GetRefCount() const{
            
            return RefCount.load(std::memory_order_acquire);
        }


	protected:
        
        DLLEXPORT friend void intrusive_ptr_add_ref(const ReferenceCounted * obj){
            
            obj->RefCount.fetch_add(1, std::memory_order_relaxed);
        }
        
        DLLEXPORT friend void intrusive_ptr_release(const ReferenceCounted * obj){
            
            if(obj->RefCount.fetch_sub(1, std::memory_order_release) == 1){
                std::atomic_thread_fence(std::memory_order_acquire);
                delete obj;
            }
        }
        
    private:

        mutable std::atomic_int_fast32_t RefCount;
	};

}


