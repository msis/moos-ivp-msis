#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <ivp/ALogScanner.h>
#include <ivp/LogUtils.h>

namespace py = boost::python;

typedef std::vector<std::string> StrVec;

py::object ALogEntry_getValue(
        ALogEntry &dis) {
    if(dis.isNull()) {
        return py::object();
    }else if(dis.isNumerical()) {
        return py::object(dis.getDoubleVal());
    }else {
        return py::object(dis.getStringVal());
    }
}

py::object ScanReport_getVarSources(
        ScanReport &dis,
        py::object param) {
    if(PyNumber_Check(param.ptr())) {
        int index = py::extract<int>(param);
        return py::str(dis.getVarSources(index)).split(",");
    }else{
        std::string varname = py::extract<std::string>(param);
        return py::str(dis.getVarSources(varname)).split(",");
    }
}
double ScanReport_getVarFirstTime(
        ScanReport &dis,
        py::object param) {
    if(PyNumber_Check(param.ptr())) {
        int index = py::extract<int>(param);
        return dis.getVarFirstTime(index);
    }else{
        std::string varname = py::extract<std::string>(param);
        return dis.getVarFirstTime(varname);
    }
}
double ScanReport_getVarLastTime(
        ScanReport &dis,
        py::object param) {
    if(PyNumber_Check(param.ptr())) {
        int index = py::extract<int>(param);
        return dis.getVarLastTime(index);
    }else{
        std::string varname = py::extract<std::string>(param);
        return dis.getVarLastTime(varname);
    }
}
unsigned int ScanReport_getVarCount(
        ScanReport &dis,
        py::object param) {
    if(PyNumber_Check(param.ptr())) {
        int index = py::extract<int>(param);
        return dis.getVarCount(index);
    }else{
        std::string varname = py::extract<std::string>(param);
        return dis.getVarCount(varname);
    }
}
unsigned int ScanReport_getVarChars(
        ScanReport &dis,
        py::object param) {
    if(PyNumber_Check(param.ptr())) {
        int index = py::extract<int>(param);
        return dis.getVarChars(index);
    }else{
        std::string varname = py::extract<std::string>(param);
        return dis.getVarChars(varname);
    }
}


class ALogEntryIter {
    public:
    ALogEntryIter(py::object file, bool allstrs) {
        fileref = (PyFileObject *) file.ptr();
        Py_INCREF((PyObject *) fileref);
        allstrings = allstrs;
    }

    ~ALogEntryIter() {
        Py_DECREF((PyObject *) fileref);
        fileref = NULL;
    }

    const ALogEntryIter *iter() {
        return this;
    }
    ALogEntry next() {
        ALogEntry entry = getNextRawALogEntry(PyFile_AsFile((PyObject *) fileref), allstrings);
        if (entry.getStatus() == "eof") {
            PyErr_SetString(PyExc_StopIteration, "done!");
            py::throw_error_already_set();
        }else if(entry.getStatus() == "invalid") {
        }
        return entry;
    }
    private:
        PyFileObject* fileref;
        bool allstrings;
};

ALogEntryIter *alogentries(py::object file, bool allstrings = 0) {
    if(PyFile_Check(file.ptr()) && 
            PyObject_GetAttrString(file.ptr(), "closed") == Py_False) {
        ALogEntryIter *iter = new ALogEntryIter(file, allstrings);
        return iter;
    }else{
        PyErr_SetString(PyExc_TypeError, 
                "next_ALogEntry wants file to be an open file object");
        return NULL;
    }
}

BOOST_PYTHON_FUNCTION_OVERLOADS(alogentries_overloads, alogentries, 1, 2)

BOOST_PYTHON_MODULE(logutils) 
{
    using namespace boost::python;
    class_<ALogScanner, boost::noncopyable>("ALogScanner",
            "Scans through an alog file, accumulating statistics about the data")
        .def("openALogFile",&ALogScanner::openALogFile, args("file_name"),
                "open the alog file\n")
        .def("scan", &ALogScanner::scan,
                "parse the alog file")
    ;
    class_<StrVec>("StringVector")
        .def(vector_indexing_suite<StrVec>());

    class_<ScanReport, boost::noncopyable>("ScanReport")
        .def("contains_var", &ScanReport::containsVar)
        .def("var_index", &ScanReport::getVarIndex)
        .def("varname", &ScanReport::getVarName)
        .def("var_sources", &ScanReport_getVarSources)
        .def("var_first_time", &ScanReport_getVarFirstTime)
        .def("var_last_time", &ScanReport_getVarLastTime)
        .def("var_count", &ScanReport_getVarCount,
                "returns number of times var was updated")
        .def("var_chars", &ScanReport_getVarChars)
        .def("max_lines", &ScanReport::getMaxLines)
        .def("max_chars", &ScanReport::getMaxChars)
        .def("max_varname_len", &ScanReport::getVarNameMaxLength)
        .def("min_start_time", &ScanReport::getMinStartTime)
        .def("max_start_time", &ScanReport::getMaxStartTime)
        .def("max_stop_time", &ScanReport::getMaxStopTime)
        .def("__len__", &ScanReport::size)
        .def("sort", &ScanReport::sort, args("style"),
                "sort data in place.\n"
                "styles: one of \n"
                "   bychars_ascending\n"
                "   bychars_descending\n"
                "   bylines_ascending\n"
                "   bylines_descending\n"
                "   bystarttime_ascending\n"
                "   bystarttime_descending\n"
                "   bystoptime_ascending\n"
                "   bystoptime_descending\n"
                "   byvars_ascending\n"
                "   byvars_descending\n"
                "   bysrc_ascending\n"
                "   bysrc_descending\n")
        .def("fill_app_stats", &ScanReport::fillAppStats,
                "Build app stat summary info")
        .def("all_sources", &ScanReport::getAllSources,
                "can use after call to self.fill_app_stats()")
        .def("lines_by_source", &ScanReport::getLinesBySource, args("src"),
                "can use after call to self.fill_app_stats()")
        .def("chars_by_source", &ScanReport::getCharsBySource, args("src"),
                "can use after call to self.fill_app_stats()")
        .def("lines_pct_by_source", &ScanReport::getLinesPctBySource, 
                args("src"),
                "gets percent of lines originating from src.\n"
                "can use after call to self.fill_app_stats()")
        .def("chars_pct_by_source", &ScanReport::getCharsPctBySource, 
                args("src"),
                "gets percent of chars originating from src.\n"
                "can use after call to self.fill_app_stats()")

        ;

    class_<ALogEntryIter, boost::noncopyable>("ALogEntryIter", init<object,bool>())
        .def("__iter__", &ALogEntryIter::iter, return_internal_reference< >())
        .def("next", &ALogEntryIter::next)
        ;
    def("alogentries", &alogentries , 
    alogentries_overloads(
        args("file", "allstrings"),
        "allstrings = true => store double values as strings\n")
        [return_value_policy<manage_new_object>()]
        );

    class_<ALogEntry>("ALogEntry")
        .add_property("time", &ALogEntry::time)
        .add_property("varname", &ALogEntry::getVarName)
        .add_property("source", &ALogEntry::getSource)
        .add_property("source_aux", &ALogEntry::getSrcAux)
        .add_property("string_val", &ALogEntry::getStringVal)
        .add_property("float_val", &ALogEntry::getDoubleVal)
        .add_property("value", &ALogEntry_getValue, 
                "return the appropriate one of string_val, float_val")
        .add_property("is_numerical", &ALogEntry::isNumerical)
        .add_property("is_null", &ALogEntry::isNull)
        .def("skew_backward", &ALogEntry::skewBackward)
        .def("skew_forward", &ALogEntry::skewForward)
        ;
}
