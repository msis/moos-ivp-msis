// from http://stackoverflow.com/questions/6776888/wrapping-a-list-of-structs-with-boost-python
// because (*^*&&* boost::python doesn't have a mapper for std::list
#include <list>
#include <algorithm>
#include <boost/bind.hpp>
#include <boost/python.hpp>

// for MOOSMSG_LIST
template<class T, class EqPred>
struct listwrap
{
    typedef typename T::value_type value_type;
    typedef typename T::const_iterator const_iterator;
    typedef typename T::iterator iter_type;

    static void add(T & x, value_type const& v)
    {
        x.push_back(v);
    }

    static bool in(T const& x, value_type const& v)
    {
        return std::find_if(x.begin(), x.end(), boost::bind(EqPred(), v, _1)) != x.end();
    }

    static int index(T const& x, value_type const& v)
    {
        int i = 0;
        for(const_iterator it=x.begin(); it!=x.end(); ++it,++i)
            if( EqPred()(*it, v)) return i;

        PyErr_SetString(PyExc_ValueError, "Value not in the list");
        throw boost::python::error_already_set();
    }

    static void del(T& x, int i)
    {
        if( i<0 ) 
            i += x.size();

        iter_type it = x.begin();
        for (int pos = 0; pos < i; ++pos)
            ++it;

        if( i >= 0 && i < (int)x.size() ) {
            x.erase(it);
        } else {
            PyErr_SetString(PyExc_IndexError, "Index out of range");
            boost::python::throw_error_already_set();
        }
    }

    static value_type& get(T& x, int i)
    {
        if( i < 0 ) 
            i += x.size();

        if( i >= 0 && i < (int)x.size() ) {
            iter_type it = x.begin(); 
            for(int pos = 0; pos < i; ++pos)
                ++it;
            return *it;                             
        } else {
            PyErr_SetString(PyExc_IndexError, "Index out of range");
            throw boost::python::error_already_set();
        }
    }

    static void set(T& x, int i, value_type const& v)
    {
        if( i < 0 ) 
            i += x.size();

        if( i >= 0 && i < (int)x.size() ) {
            iter_type it = x.begin(); 
            for(int pos = 0; pos < i; ++pos)
                ++it;
            *it = v;
        } else {
            PyErr_SetString(PyExc_IndexError, "Index out of range");
            boost::python::throw_error_already_set();
        }
    }
};


#define EXPORT_STLLIST(T,EqPred,typeName) \
boost::python::class_<std::list<T> >(typeName) \
        .def("__len__", &std::list<T>::size) \
        .def("clear", &std::list<T>::clear) \
        .def("append", &listwrap<std::list<T>,EqPred>::add, \
            with_custodian_and_ward<1,2>()) /* to let container keep value */ \
        .def("__getitem__", &listwrap<std::list<T>,EqPred>::get, \
            return_value_policy<copy_non_const_reference>()) \
        .def("__setitem__", &listwrap<std::list<T>, EqPred>::set, \
            with_custodian_and_ward<1,2>()) /* to let container keep value */ \
        .def("__delitem__", &listwrap<std::list<T>,EqPred>::del) \
        .def("__contains__", &listwrap<std::list<T>,EqPred>::in) \
        .def("__iter__", iterator<std::list<T> >()) \
        .def("index", &listwrap<std::list<T>,EqPred>::index);
template<class T, class EqPred>
boost::python::class_<std::list<T> > export_STLList(const char* typeName)
{
    using namespace boost::python;

    class_<std::list<T> > result(typeName);
        result.def("__len__", &std::list<T>::size)
        .def("clear", &std::list<T>::clear)
        .def("append", &listwrap<std::list<T>,EqPred>::add,
            with_custodian_and_ward<1,2>()) // to let container keep value
        .def("__getitem__", &listwrap<std::list<T>,EqPred>::get,
            return_value_policy<copy_non_const_reference>())
        .def("__setitem__", &listwrap<std::list<T>, EqPred>::set,
            with_custodian_and_ward<1,2>()) // to let container keep value
        .def("__delitem__", &listwrap<std::list<T>,EqPred>::del)
        .def("__contains__", &listwrap<std::list<T>,EqPred>::in)
        .def("__iter__", iterator<std::list<T> >())
        .def("index", &listwrap<std::list<T>,EqPred>::index);
        return result;
}
