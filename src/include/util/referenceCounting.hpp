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
#include <type_traits>

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

		ReferencedObject* retain() { assert(retentionCount); retentionCount++; return this; }
		const ReferencedObject* retain() const { assert(retentionCount); retentionCount++; return this; }
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
template<class T> class Retain;
template<class T>
	class Release {
		public:
			explicit Release(T* obj) : itsObject(obj) { };
			Release(const Release<T>&) = delete;

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
			template<class U> bool operator ==(const Release<U>& other) const { return (char*)this->itsObject == (char*)&*other; };
			template<class U> bool operator !=(const Release<U>& other) const { return (char*)this->itsObject != (char*)&*other; };
			operator T*() const { return itsObject; };

			//Tell the compiler that it can just use a Release<Derived> in place of a Release<Base>.
			template<class U, typename = typename std::enable_if<std::is_convertible<T*, U*>::value>::type> operator Retain<U>&() { return *(Retain<U>*)this; };
			template<class U, typename = typename std::enable_if<std::is_convertible<T*, U*>::value>::type> operator const Retain<U>&() const { return *(const Retain<U>*)this; };
			template<class U, typename = typename std::enable_if<std::is_convertible<U*, T*>::value>::type> Release<T>& operator=(const Release<U>& obj) {
				setObject(obj.itsObject);
				return *this;
			};

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
			Retain(T* obj = 0) : Release<T>(obj) {	//To construct with a bare pointer, use `Retain<T> foo(bar);`
				if (obj) obj->retain();
			};
			Retain(const Retain<T>& obj) : Release<T>((T*)&*obj) {	//To construct with a smart pointer, use `Retain<T> foo = bar;`
				if (obj) ((T*)&*obj)->retain();
			};

			Retain<T>& operator=(T* obj) {
				if (obj) obj->retain();
				if (this->itsObject) this->itsObject->release();
				this->itsObject = obj;
				return *this;
			};

			//Tell the compiler that it can just use a Retain<Derived> in place of a Retain<Base>.
			template<class U, typename = typename std::enable_if<std::is_convertible<T*, U*>::value>::type> operator Retain<U>&() { return *(Retain<U>*)this; };
			template<class U, typename = typename std::enable_if<std::is_convertible<T*, U*>::value>::type> operator const Retain<U>&() const { return *(const Retain<U>*)this; };
			template<class U, typename = typename std::enable_if<std::is_convertible<U*, T*>::value>::type> Retain<T>& operator=(const Release<U>& obj) {
				this->setObject((T*)&*obj);
				return *this;
			};
	};

#endif
