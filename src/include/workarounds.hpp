//This file is for workarounds for problems in other peoples code that we have to fix/workaround. As such, any code in here should at one point or another become obsolete.

#ifndef INCLUDE_GUARD_WORKAROUNDS_HPP
#define INCLUDE_GUARD_WORKAROUNDS_HPP

//Unfortunately, the STL is too posh to provide hash values for its containers, so, here is at least a fix for pairs.
//This is necessary to be able to use a pair as a key in an unordered_map.
//XXX: This will likely lead to problems, should the STL realize their shame and provide something like this themselves; the danger is that code compiled with such a fixed version of the STL might not link against a siox version with this fix. But until they do provide a fix, the only alternative would be not to use unordered_map with more complex keys, which is not really an option.
namespace std {
	template <class T, class U> struct hash<pair<T, U> > {
		size_t operator()(const pair<T, U>& me) const {
			return hash<T>()(me.first)*31 + hash<U>()(me.second);
		}
	};
}

//Out of the box, it is not possible to do arithmetic on enum types, making it _very_ tedious to iterate through all values of an enum. So these macros allow for easy overloading of the relevant operators.
//Currently, this will define the +, -, and pre-/postfix ++/-- operators.
#define ADD_ENUM_OPERATORS(ENUM_NAME) \
	inline ENUM_NAME operator+(ENUM_NAME a, int b) { return (ENUM_NAME)((int)a + b); } \
	inline ENUM_NAME operator+(int a, ENUM_NAME b) { return (ENUM_NAME)(a + (int)b); } \
	inline ENUM_NAME operator-(ENUM_NAME a, int b) { return (ENUM_NAME)((int)a - b); } \
	inline ENUM_NAME operator-(int a, ENUM_NAME b) { return (ENUM_NAME)(a - (int)b); } \
	inline int operator-(ENUM_NAME a, ENUM_NAME b) { return (int)a - (int)b; } \
	inline ENUM_NAME operator++(ENUM_NAME& a) { \
		return a = (ENUM_NAME)((int)a + 1); \
	} \
	inline ENUM_NAME operator--(ENUM_NAME& a) { \
		return a = (ENUM_NAME)((int)a - 1); \
	} \
	inline ENUM_NAME operator++(ENUM_NAME& a, int) { \
		ENUM_NAME result = a; \
		a = (ENUM_NAME)((int)a + 1); \
		return result; \
	} \
	inline ENUM_NAME operator--(ENUM_NAME& a, int) { \
		ENUM_NAME result = a; \
		a = (ENUM_NAME)((int)a - 1); \
		return result; \
	}

#endif
