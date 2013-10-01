//This file is for workarounds for problems in other peoples code that we have to fix/workaround. As such, any code in here should at one point or another become obsolete.

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

