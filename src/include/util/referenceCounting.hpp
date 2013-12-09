/**
 * Support for reference counted objects.
 *
 * @date 2012,2013
 * @author Nathanael Hübbe
 */

#ifndef INCLUDE_GUARD_REFERENCE_COUNTING
#define INCLUDE_GUARD_REFERENCE_COUNTING

#include <stdint.h>
#include <atomic>
#include <assert.h>

//Select a type to use for the reference counts.
//We prefer it to be guaranteed to be lock free, and if that fails, we select one that can at least be lock free. In either case, we want the largest type that is available (overflowing reference counts just won't do!).
//#define OUTPUT_REFCOUNTTYPE
#if ATOMIC_POINTER_LOCK_FREE == 2
	typedef uintptr_t RefCountType;
	#ifdef OUTPUT_REFCOUNTTYPE
		#warning RefCountType = uintptr_t, guaranteed lockfree
	#endif
#elif ATOMIC_LLONG_LOCK_FREE == 2
	typedef unsigned long long RefCountType;
	#ifdef OUTPUT_REFCOUNTTYPE
		#warning RefCountType = unsigned long long, guaranteed lockfree
	#endif
#elif ATOMIC_LONG_LOCK_FREE == 2
	typedef unsigned long RefCountType;
	#ifdef OUTPUT_REFCOUNTTYPE
		#warning RefCountType = unsigned long, guaranteed lockfree
	#endif
#elif ATOMIC_INT_LOCK_FREE == 2
	typedef unsigned RefCountType;
	#ifdef OUTPUT_REFCOUNTTYPE
		#warning RefCountType = unsigned, guaranteed lockfree
	#endif
#elif ATOMIC_POINTER_LOCK_FREE == 1
	typedef uintptr_t RefCountType;
	#ifdef OUTPUT_REFCOUNTTYPE
		#warning RefCountType = uintptr_t, lockfree
	#endif
#elif ATOMIC_LLONG_LOCK_FREE == 1
	typedef unsigned long long RefCountType;
	#ifdef OUTPUT_REFCOUNTTYPE
		#warning RefCountType = unsigned long long, lockfree
	#endif
#elif ATOMIC_LONG_LOCK_FREE == 1
	typedef unsigned long RefCountType;
	#ifdef OUTPUT_REFCOUNTTYPE
		#warning RefCountType = unsigned long, lockfree
	#endif
#elif ATOMIC_INT_LOCK_FREE == 1
	typedef unsigned RefCountType;
	#ifdef OUTPUT_REFCOUNTTYPE
		#warning RefCountType = unsigned, lockfree
	#endif
#else
	typedef uintptr_t RefCountType;
	#ifdef OUTPUT_REFCOUNTTYPE
		#warning RefCountType = uintptr_t, not lockfree!
	#endif
#endif

/**
 * ReferencedObject
 *
 * A base class for reference counted objects. Usually used in conjunction with Retain<> smart pointers.
 */

class ReferencedObject {
	public:
		ReferencedObject() : retentionCount(1) {}
		ReferencedObject(const ReferencedObject& obj) : retentionCount(1) {}
		ReferencedObject& operator=(const ReferencedObject& source) { return *this; };	//no need to call it from overriding version - it's not functional

		ReferencedObject* retain() { RefCountType oldCount = retentionCount++; assert(oldCount); return this; }
		const ReferencedObject* retain() const { RefCountType oldCount = retentionCount++; assert(oldCount); return this; }
		void release() const {
			RefCountType oldCount = retentionCount.fetch_sub(1);
			assert(oldCount);
			if(oldCount == 1) delete this;
		}
		intmax_t references() const { return retentionCount; }

		virtual ~ReferencedObject() {}
	private:
		mutable std::atomic<RefCountType> retentionCount;
};

/**
 * Release
 *
 * a pointer class that releases its object when the pointer object is destructed
 * reinventing auto_ptr, but this time for retention counting
 */

template<class T>
	class Release {
		public:
			explicit Release(T* obj) : itsObject(obj) { };
			T& operator*() { return *itsObject; };
			const T& operator*() const { return *itsObject; };
			T* operator->() { return itsObject; };
			const T* operator->() const { return itsObject; };
			void setObject(T* obj) {
				if (obj) obj->retain();
				if (this->itsObject) this->itsObject->release();
				this->itsObject = obj;
			};
			Release<T>& operator=(T* obj) {
				setObject(obj);
				return *this;
			};
			Release<T>& operator=(const Release<T>& obj) {
				setObject(obj.itsObject);
				return *this;
			};
			Release<T>& release(T* obj) {
				if (this->itsObject) this->itsObject->release();
				this->itsObject = obj;
				return *this;
			};
			Release<T>& release(const Release<T>& obj) {
				if (this->itsObject) this->itsObject->release();
				this->itsObject = obj.itsObject;
				return *this;
			};
			Release<T>& copy(const T& obj) {
				return release(new T(obj));
			};
			operator bool() const { return itsObject; };
			operator T*() const { return itsObject; };
			~Release() { if (itsObject) itsObject->release(); };
		protected:
			T* itsObject;
	};

template<class T>
	inline void exclusive(Release<T>& ptr) {
		if (ptr->references() != 1) {
			Release<T> newObj(new T(*ptr));
			ptr = newObj;
		};
	}

/**
 * Retain
 *
 * a pointer class that takes ownership for its object while it lives, retaining upon construction & assignment and releasing upon assignment & destruction
 * reinventing the wheel once more, but this time it is a pretty wheel!
 */

template<class T>
	class Retain : public Release<T> {
		public:
			Retain(T* obj = 0) : Release<T>(obj) {
				if (obj) obj->retain();
			};
			Retain(const Release<T>& obj) : Release<T>(obj) {
				if (obj) ((Retain<T>&)obj)->retain();
			};
			Retain(const Retain<T>& obj) : Release<T>(obj) {
				if (obj) ((Retain<T>&)obj)->retain();
			};
			Retain<T>& operator=(T* obj) {
				if (obj) obj->retain();
				if (this->itsObject) this->itsObject->release();
				this->itsObject = obj;
				return *this;
			};
			Retain<T>& operator=(const Release<T>& obj) {
				if (obj) ((Release<T>&)obj)->retain();
				if (this->itsObject) this->itsObject->release();
				this->itsObject = (T*)(const T*)obj;
				return *this;
			};
	};

#endif
