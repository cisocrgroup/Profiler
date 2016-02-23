#ifndef csl_Charvec_h__
#define csl_Charvec_h__

#include <map>

namespace csl {
        class Charvec {
        public:
                typedef wchar_t key_type;
                typedef unsigned long long value_type;
                typedef std::map<key_type, value_type> map_type;
                typedef map_type::const_iterator const_iterator;
                typedef map_type::iterator iterator;

                Charvec(): map_() {}
                const_iterator begin() const {return map_.begin();}
                iterator begin()  {return map_.begin();}
                const_iterator end() const {return map_.end();}
                iterator end()  {return map_.end();}
                value_type& operator[](key_type k) {
                        return map_[k];
                }
                value_type operator[] (key_type k) const {
                        const_iterator i = map_.find(k);
                        return i == map_.end() ? ZERO : i->second;
                }
                void clear() {
                        map_.clear();
                }

        private:
                static const value_type ZERO = 0;
                map_type map_;
        };
}

#endif // csl_Charvec_h__
