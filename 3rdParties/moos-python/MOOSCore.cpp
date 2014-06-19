#include <boost/python.hpp>
#include <MOOS/libMOOS/MOOSLib.h>
#include "std_item.hpp"
#include "ivp/MBUtils.h"

namespace py = boost::python;

class CMOOSAppWrap : public CMOOSApp, public py::wrapper<CMOOSApp>
{
    protected:
        bool Iterate() {
            return publicIterate();
        }
        bool OnNewMail(MOOSMSG_LIST & NewMail) {
            return publicOnNewMail(NewMail);
        }
        bool OnStartUp() {
            return publicOnStartUp();
        }

    public:
        CMOOSAppWrap() {
        }

        virtual ~CMOOSAppWrap() {
        }

        CMOOSCommClient &Comms() {
            return m_Comms;
        }

        CProcessConfigReader &MissionReader() {
            return m_MissionReader;
        }

        std::string AppName() {
            return GetAppName();
        }

        void publicSetAppFreq(double dfFreq, double dfMaxFreq = 0.0) {
            SetAppFreq(dfFreq, dfMaxFreq); 
        }
        bool publicSetCommsFreq(unsigned int nFreq) {
            return SetCommsFreq(nFreq);
        }
        bool publicIterate() {
            return this->get_override("iterate")();
        }
        bool publicOnNewMail(MOOSMSG_LIST &NewMail) {
            return this->get_override("onNewMail")(NewMail);
        }
        bool publicOnStartUp() {
            return this->get_override("onStartUp")();
        }
        double publicGetAppStartTime() {
            return this->GetAppStartTime();
        }
        bool OnConnectToServer() {
            return this->get_override("onConnectToServer")();
        }
        bool OnDisconnectFromServer() {
            return this->get_override("onDisconnectFromServer")();
        }

};

class CMOOSInstrumentWrap : public CMOOSInstrument, public py::wrapper<CMOOSInstrument>
{
    protected:
        bool Iterate() {
            return publicIterate();
        }
        bool OnNewMail(MOOSMSG_LIST & NewMail) {
            return publicOnNewMail(NewMail);
        }
        bool OnStartUp() {
            return publicOnStartUp();
        }

    public:
        CMOOSInstrumentWrap() {
        }
        virtual ~CMOOSInstrumentWrap() {
        }

        CMOOSCommClient &Comms() {
            return m_Comms;
        }

        CProcessConfigReader &MissionReader() {
            return m_MissionReader;
        }

        CMOOSSerialPort &Port() {
            return m_Port;
        }

        std::string AppName() {
            return GetAppName();
        }

        void publicSetAppFreq(double dfFreq, double dfMaxFreq = 0.0) {
            SetAppFreq(dfFreq, dfMaxFreq); 
        }
        bool publicSetCommsFreq(unsigned int nFreq) {
            return SetCommsFreq(nFreq);
        }
        bool publicIterate() {
            return this->get_override("iterate")();
        }
        bool publicOnNewMail(MOOSMSG_LIST &NewMail) {
            return this->get_override("onNewMail")(NewMail);
        }
        bool publicOnStartUp() {
            return this->get_override("onStartUp")();
        }
        double publicGetAppStartTime() {
            return this->GetAppStartTime();
        }
        bool OnConnectToServer() {
            return this->get_override("onConnectToServer")();
        }
        bool OnDisconnectFromServer() {
            return this->get_override("onDisconnectFromServer")();
        }

};

bool MOOSApp_Run(
        py::object dis,
        const std::string &sName, 
        const std::string &sMissionFile = "Mission.moos",
        py::list argv = py::list())
{
    py::extract<CMOOSAppWrap &> app_dis(dis);
    py::extract<CMOOSInstrumentWrap &> instr_dis(dis);
    if (!app_dis.check() && !instr_dis.check()) return false;
    size_t argc = len(argv);
    if (argv == py::list()) {
        if(app_dis.check()) {
            return app_dis().Run(sName, sMissionFile);
        }else{
            return instr_dis().Run(sName, sMissionFile);
        }
    }else{
        char **_argv = new char*[argc];
        for(size_t i = 0; i < argc; i++) {
            _argv[i] = py::extract<char *>(argv[i]);
        }
        bool result = 0;
        if(app_dis.check()) {
            result = app_dis().Run(sName, sMissionFile);
        }else{
            result = instr_dis().Run(sName, sMissionFile);
        }
        delete _argv;
        return result;
    }
}

bool CMOOSCommClient_pyNotify(
        CMOOSCommClient &dis,
        const std::string &sVar, 
        py::object val, 
        py::object SrcAux = py::object(), 
        double dfTime=-1, 
        bool IsBinary=false) {

    PyObject *val_ptr = val.ptr();
    if(PyNumber_Check(val_ptr)) {
        if(SrcAux.ptr() != Py_None) {
            return dis.Notify(sVar, 
                    py::extract<double>(val), 
                    py::extract<std::string>(SrcAux), dfTime);
        }else{
            double d = py::extract<double>(val);
            return dis.Notify(sVar, d, dfTime);
        }
    }else if(PyObject_IsInstance(val_ptr, (PyObject*)&PyString_Type)) {
        if(IsBinary) {
            char *data = py::extract<char *>(val);
            size_t size = len(val);
            if(SrcAux.ptr() == Py_None) {
                return dis.Notify(sVar, data, size, dfTime);
            }else{
                return dis.Notify(sVar, data, size, 
                        py::extract<std::string>(SrcAux), dfTime);
            }
        }else{
            if(SrcAux.ptr() == Py_None) {
                return dis.Notify(sVar, py::extract<std::string>(val), dfTime);
            }else{
                return dis.Notify(sVar, py::extract<std::string>(val), 
                        py::extract<std::string>(SrcAux), dfTime);
            }
        }
    }else{
        PyErr_SetString(PyExc_TypeError, 
                "Notify wants val to be a number or a string");
        return false;
    }

}

py::object MOOSMSG_LIST_PeekMail(
        MOOSMSG_LIST &Mail,
        const std::string &sKey,
        bool bErase = false,
        bool bFindYoungest = false) {

    CMOOSMsg result_msg;
    if(CMOOSCommClient::PeekMail(Mail, sKey, result_msg, bErase, bFindYoungest))
    {
        return py::object(result_msg);
    }
    return py::object();
}

py::object MOOSMsg_GetBinaryData(CMOOSMsg msg) {
    unsigned char *data = msg.GetBinaryData();
    if(data == NULL) {
        return py::object();
    }else{
        size_t size = msg.GetBinaryDataSize();
        return py::str((char const*) data,size);
    }
}
std::string MOOSMsg_str(CMOOSMsg &dis) {
    return dis.GetAsString();
}

py::object MOOSMsg_GetValue(CMOOSMsg msg) {
    if(msg.IsDouble()) {
        return py::make_tuple(msg.GetDouble(),msg.GetDoubleAux());
    }else if(msg.IsString() || msg.IsBinary()) {
        return py::object(msg.GetString());
    }
    else return py::object();
}

py::tuple MOOSMsg_GetSource(CMOOSMsg msg) {
    return py::make_tuple(msg.GetSource(), msg.GetSourceAux());
}

PyTypeObject *obj2astype(py::object AsType) {
    if(AsType.ptr() == Py_None) {
        return &PyString_Type;
    }else if(PyObject_IsInstance(AsType.ptr(), (PyObject*) &PyType_Type)) {
        return (PyTypeObject *) AsType.ptr();
    }else{
        return NULL;
    }
}

bool IsType(PyTypeObject* mytype, PyTypeObject *thetype) {
    return mytype == thetype || PyType_IsSubtype(mytype, thetype);
}

py::tuple CProcessConfigReader_GetConfigurationParam(
        CProcessConfigReader &dis,
        std::string sParam,
        py::object AppName = py::object(),
        py::object AsType = py::object()
        ) {

        PyTypeObject *astype = obj2astype(AsType);
        if(astype == NULL) {
            PyErr_SetString(PyExc_TypeError, "AsType must be a python type");
            return py::make_tuple(false, py::object());
        }

        if(AppName.ptr() == Py_None) {
            if(IsType(astype,&PyFloat_Type)) {
                double val;
                bool result = dis.GetConfigurationParam(sParam, val);
                return py::make_tuple(result, val);
            }else if(IsType(astype,&PyInt_Type)) {
                int val;
                bool result = dis.GetConfigurationParam(sParam, val);
                return py::make_tuple(result, val);
            }else if(IsType(astype,&PyBool_Type)) {
                bool val;
                bool result = dis.GetConfigurationParam(sParam, val);
                return py::make_tuple(result, val);
            }else if(IsType(astype,&PyList_Type)) {
                // todo: numpyfy this
                std::vector<double> val;
                int rows;
                int cols;
                bool result = dis.GetConfigurationParam(sParam, val,rows, cols);
                return py::make_tuple(result, val,rows, cols);
            }else if(IsType(astype,&PyString_Type)) {
                std::string val;
                bool result = dis.GetConfigurationParam(sParam, val);
                return py::make_tuple(result, val);
            }else{
                PyErr_SetString(PyExc_TypeError, "AsType unhandled type");
                return py::make_tuple(false, py::object());
            }
        }else{
            std::string appname = py::extract<std::string>(AppName);
            if(IsType(astype,&PyFloat_Type)) {
                double val;
                bool result = dis.GetConfigurationParam(appname, sParam, val);
                return py::make_tuple(result, val);
            }else if(IsType(astype,&PyInt_Type)) {
                int val;
                bool result = dis.GetConfigurationParam(appname, sParam, val);
                return py::make_tuple(result, val);
            }else if(IsType(astype,&PyBool_Type)) {
                bool val;
                bool result = dis.GetConfigurationParam(appname, sParam, val);
                return py::make_tuple(result, val);
            }else if(IsType(astype,&PyString_Type)) {
                std::string val;
                bool result = dis.GetConfigurationParam(appname, sParam, val);
                return py::make_tuple(result, val);
            }else{
                PyErr_SetString(PyExc_TypeError, "AsType unhandled type");
                return py::make_tuple(false, py::object());
            }
        }
}

py::dict CProcessConfigReader_GetConfiguration(
        CProcessConfigReader &dis,
        std::string AppName
        ) {

        STRING_LIST list;
        py::dict result;
        dis.GetConfiguration(AppName, list);
        for(STRING_LIST::iterator it = list.begin(); it != list.end(); it++) {
            std::string line = *it;
            std::string varname = stripBlankEnds(MOOSChomp(line, "="));
            std::string val = stripBlankEnds(line);
            if(isNumber(val)) {
                result[varname] = py::object(atof(val.c_str()));
            }else if(isBoolean(val)) {
                bool boolval = false;
                setBooleanOnString(boolval, val, true);
                result[varname] = py::object(boolval);
            }else{
                result[varname] = py::object(val);
            }
        }
        return result;
}


py::object CMOOSFileReader_GetValue(
        CMOOSFileReader &dis,
        std::string sName,
        py::object AsType = py::object()
        ) {

    PyTypeObject *astype = obj2astype(AsType);
    if(astype == NULL) {
        PyErr_SetString(PyExc_TypeError, "as_type must be a python type");
        return py::make_tuple(false, py::object());
    }

    bool result = false;
    py::object ret;

    if(IsType(astype,&PyFloat_Type)) {
        double val;
        result = dis.GetValue(sName, val);
        if(result) ret = py::object(val);
    }else if(IsType(astype,&PyInt_Type)) {
        int val;
        result = dis.GetValue(sName, val);
        if(result) ret = py::object(val);
    }else if(IsType(astype,&PyBool_Type)) {
        bool val;
        result = dis.GetValue(sName, val);
        if(result) ret = py::object(val);
    }else if(IsType(astype,&PyString_Type)) {
        std::string val;
        result = dis.GetValue(sName, val);
        if(result) ret = py::object(val);
    }else{
        PyErr_SetString(PyExc_TypeError, "as_type: unhandled type");
        return py::object();
    }

    if(!result) {
        PyErr_SetString(PyExc_Exception, "MOOSFileReader.GetValue failed");
    }
    return ret;
}

py::object CMOOSSerialPort_GetTelegram(
        CMOOSSerialPort &dis,
        double dfTimeOut) {
    std::string result;
    if(dis.GetTelegram(result, dfTimeOut)) {
        return py::str(result);
    }else{
        return py::object();
    }
}
            //
py::object CMOOSSerialPort_ReadNWithTimeOut(
        CMOOSSerialPort &dis,
        size_t nBufferLen, 
        double Timeout=0.5) {
    char *buf = (char *) malloc(nBufferLen);
    size_t didRead = dis.ReadNWithTimeOut(buf, nBufferLen, Timeout);
    return py::str(buf, didRead);
}

int CMOOSSerialPort_Write(
        CMOOSSerialPort &dis,
        py::str Str) {
    return dis.Write(py::extract<const char*>(Str), len(Str));
}

struct Msg_equals {
    typedef bool result_type;

    bool operator()(const CMOOSMsg &msg1, const CMOOSMsg &msg2)
    {
        if(msg1.GetType() != msg2.GetType()) return false;
        if(msg1.GetName() != msg2.GetName()) return false;
        if(msg1.GetTime() != msg2.GetTime()) return false;
        if(msg1.IsDouble() ^ msg2.IsDouble()) return false;
        if(msg1.IsDouble() && (msg1.GetDouble() != msg2.GetDouble() || 
                    msg1.GetDoubleAux() != msg2.GetDoubleAux())) return false;
        if(msg1.IsString() ^ msg2.IsString()) return false;
        if(msg1.IsBinary() ^ msg2.IsBinary()) return false;
        if((msg1.GetString() != msg2.GetString())) return false;
        return true;
    }
};

struct iMsg_equals {
    typedef bool result_type;

    bool operator()(char *msg1, char *msg2)
    {
        return strcmp(msg1,msg2) == 0;
    }
};

PyObject *Poco_Exception_Type = NULL;
void SpitYeBloodyMessageOutAlready(Poco::Exception const &e) {
    assert(Poco_Exception_Type != NULL);
    py::object pyexc(e);
    std::string mssg = "Poco::Exception: ";
    mssg.append(e.message());
    PyErr_SetObject(Poco_Exception_Type, py::object(mssg).ptr());
}

bool (CMOOSCommClient::*Register1)(const std::string &, double) = &CMOOSCommClient::Register;
BOOST_PYTHON_FUNCTION_OVERLOADS(MOOSApp__Run, MOOSApp_Run, 2, 4)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(CMOOSCommClient__Register, Register, 1, 2)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(CMOOSMsg__GetAsString, CMOOSMsg::GetAsString, 0, 2)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(CMOOSApp__publicSetAppFreq, publicSetAppFreq, 1, 2)
BOOST_PYTHON_FUNCTION_OVERLOADS(CMOOSFileReader__GetValue,CMOOSFileReader_GetValue, 2, 3)
BOOST_PYTHON_FUNCTION_OVERLOADS(CProcessConfigReader__GetConfigurationParam,CProcessConfigReader_GetConfigurationParam,2,4)
BOOST_PYTHON_FUNCTION_OVERLOADS(CMOOSCommClient__Notify, CMOOSCommClient_pyNotify, 3, 6)
BOOST_PYTHON_FUNCTION_OVERLOADS(MOOSMSG_LIST__PeekMail, MOOSMSG_LIST_PeekMail, 2, 4)
BOOST_PYTHON_FUNCTION_OVERLOADS(MOOSTime_Overload, MOOSTime, 0, 1)
BOOST_PYTHON_FUNCTION_OVERLOADS(CMOOSSerialPort__ReadNWithTimeOut,CMOOSSerialPort_ReadNWithTimeOut, 2, 3)

BOOST_PYTHON_MODULE(core)
{
    using namespace boost::python;
    docstring_options local_docstring_options(true, true, false);


    class_<CMOOSAppWrap, CMOOSAppWrap*, boost::noncopyable>("MOOSApp")
        .def("iterate",&CMOOSAppWrap::publicIterate, 
                "called when the application should iterate. Overload this\n"
                "function in a derived class and within it write all the\n"
                "application specific code. It will be called at approximately\n"
                "nFreq = 1/AppTick Hz\nReturns: heck i dunno")
        .def("onNewMail", &CMOOSAppWrap::publicOnNewMail, args("NewMail"),
                "called when new mail has arrived. Overload this method in a derived class to process new mail.\n"
                "It will be called at approximately 1/CommsTick Hz. In this function you'll most likely interate over the\n"
                "collection of mail message received or call a m_Comms::PeekMail() to look for a specific named message.\n"
                "@param NewMail a list of new mail messages")
        .def("onStartUp", &CMOOSAppWrap::publicOnStartUp,
                "called just before the main app loop is entered. Specific initialisation code can be written\n"
                "in an overloaded version of this function")
        .def("onConnectToServer", &CMOOSAppWrap::OnConnectToServer,
                "Called when the class has succesfully connected to the server. Overload this function\n"
                "and place use it to register for notification when variables of interest change ")
        .def("onDisconnectFromServer", &CMOOSAppWrap::OnDisconnectFromServer,
                "Called when the class has disconnects from the server.")
        .def("run", &MOOSApp_Run, 
                MOOSApp__Run(
                    args("name", "mission_file", "argv"),
                    "called to start the application\n"
                    "@param sName The name of this application (used to read configuration from a\n"
                    "   mission file and if sSubscribeName is NULL, to register with the MOOSDB)\n"
                    "@param the name of the mission file\n"
                    "@param command line args"))
        .add_property("startTime",&CMOOSAppWrap::publicGetAppStartTime)
        .def("setAppFreq",&CMOOSAppWrap::publicSetAppFreq,
                CMOOSApp__publicSetAppFreq(args("freq","maxfreq"),
                    "Set the time between calls of Iterate (which is where \n"
                    "you'll probably do Application work)\n"
                    "\nCan be set using the AppTick flag in the config file"))
        .def("setCommsFreq", &CMOOSAppWrap::publicSetCommsFreq, 
                args("freq"),
                "Set the time between calls into the DB\n"
                "Can be set using the CommsTick flag in the config file")
        .add_property("app_name", &CMOOSAppWrap::AppName)
        .add_property("comms", 
                make_function(&CMOOSAppWrap::Comms, 
                    return_internal_reference<1>()),
                "The MOOS comms node. All communications happen by way of this object")
        .add_property("mission_reader", 
                make_function(&CMOOSAppWrap::MissionReader, 
                    return_internal_reference<1>()),
                "reader for mission file")
        ;



    class_<CMOOSInstrumentWrap, CMOOSInstrumentWrap*, boost::noncopyable>("MOOSInstrument")
        .def("iterate",&CMOOSInstrumentWrap::publicIterate, 
                "called when the application should iterate. Overload this\n"
                "function in a derived class and within it write all the\n"
                "application specific code. It will be called at approximately\n"
                "nFreq = 1/AppTick Hz\n"
                "Returns: heck i dunno")
        .def("onNewMail", &CMOOSInstrumentWrap::publicOnNewMail, args("NewMail"),
                "called when new mail has arrived. Overload this method in a derived class to process new mail.\n"
                "It will be called at approximately 1/CommsTick Hz. In this function you'll most likely interate over the\n"
                "collection of mail message received or call a m_Comms::PeekMail() to look for a specific named message.\n"
                "@param NewMail a list of new mail messages")
        .def("onStartUp", &CMOOSInstrumentWrap::publicOnStartUp,
                "called just before the main app loop is entered. Specific initialisation code can be written\n"
                "in an overloaded version of this function")
        .def("onConnectToServer", &CMOOSInstrumentWrap::OnConnectToServer,
                "Called when the class has succesfully connected to the server. Overload this function\n"
                "and place use it to register for notification when variables of interest change ")
        .def("onDisconnectFromServer", &CMOOSInstrumentWrap::OnDisconnectFromServer,
                "Called when the class has disconnects from the server.")
        .def("run", &MOOSApp_Run,
                MOOSApp__Run( args("name", "mission_file", "argv"),
                "called to start the application\n"
                "@param sName The name of this application (used to read configuration from a\n"
                "mission file and if sSubscribeName is NULL, to register with the MOOSDB)\n"
                "@param the name of the mission file\n"
                "@param command line args"))
        .add_property("startTime",&CMOOSInstrumentWrap::publicGetAppStartTime)
        .def("setAppFreq",&CMOOSInstrumentWrap::publicSetAppFreq,
                CMOOSApp__publicSetAppFreq(args("freq","maxfreq"),
                    "Set the time between calls of Iterate (which is where \n"
                    "you'll probably do Application work)\n"
                    "\nCan be set using the AppTick flag in the config file"))
        .def("setCommsFreq", &CMOOSInstrumentWrap::publicSetCommsFreq,
                args("freq"),
                "Set the time between calls into the DB\n"
                "Can be set using the CommsTick flag in the config file")
        .add_property("app_name", &CMOOSInstrumentWrap::AppName)
        .add_property("port", 
                make_function(&CMOOSInstrumentWrap::Port, 
                    return_internal_reference<1>()),
                "a sensor port")
        .add_property("comms", 
                make_function(&CMOOSInstrumentWrap::Comms, 
                    return_internal_reference<1>()),
                "The MOOS comms node. All communications happen by way of this object")
        .add_property("mission_reader", 
                make_function(&CMOOSInstrumentWrap::MissionReader, 
                    return_internal_reference<1>()),
                    "reader for mission file")
        ;

    class_<CMOOSFileReader, CMOOSFileReader*, boost::noncopyable>("MOOSFileReader")
        .def("getValue",&CMOOSFileReader_GetValue,
                CMOOSFileReader__GetValue(args("name","as_type"),
                    "find \"name=value\" pair in whole file;\n"
                    "returns value\n"
                    "as_type: specify float,int,bool, or str to get value as that type\n"
                    "throws an exception if retrieval fails.")
        );

    class_<CProcessConfigReader, CProcessConfigReader*, bases<CMOOSFileReader>, boost::noncopyable>("CProcessConfigReader")
        .def("app_name", &CProcessConfigReader::GetAppName)
        .def("file_name", &CProcessConfigReader::GetFileName)
        .def("configuration_param", &CProcessConfigReader_GetConfigurationParam,
                CProcessConfigReader__GetConfigurationParam(
                    args("param","app_name", "type"),
                    "read configuration parameter.\n"
                    "param: name of parameter to read.\n"
                    "app_name: name of process block to read (default: self.app_name)\n"
                    "as_type: specify float,int,bool,list,str to get result as that type\n"
                    "       (list expects raw value to be comma delimited floats)\n"
                    "Returns: the read value"))
        .def("configuration", &CProcessConfigReader_GetConfiguration)
        ;


    class_<CMOOSCommClient, CMOOSCommClient*, boost::noncopyable>("MOOSCommClient")
        .def("notify", &CMOOSCommClient_pyNotify,
        CMOOSCommClient__Notify( args("var","val","SrcAux","dfTime","IsBinary"),
        "notify the MOOS community that something has changed (string)\n"
        "@param var name of variable being changed\n"
        "@param val value of variable being changed (str or float)\n"
        "@param SrcAux ??\n"
        "@param dfTime ??\n"
        "@param IsBinary when val is str, specify data type be Binary, not String"
        ""))
        .def("register", Register1,
                CMOOSCommClient__Register( args("var","interval"),
                "Register for notification in changes of named variable\n" 
                "@param var name of variable of interest\n" 
                "@param interval minimum time between notifications"))
        .add_property("community", &CMOOSCommClient::GetCommunityName,
                "name of community client is attached to")
                ;

    // Message types
    scope().attr("MOOS_NOTIFY") = MOOS_NOTIFY;
    scope().attr("MOOS_REGISTER") = MOOS_REGISTER;
    scope().attr("MOOS_UNREGISTER") = MOOS_UNREGISTER;
    scope().attr("MOOS_WILDCARD_REGISTER") = MOOS_WILDCARD_REGISTER;
    scope().attr("MOOS_NOT_SET") = MOOS_NOT_SET;
    scope().attr("MOOS_COMMAND") = MOOS_COMMAND;
    scope().attr("MOOS_ANONYMOUS") = MOOS_ANONYMOUS;
    scope().attr("MOOS_NULL_MSG") = MOOS_NULL_MSG;
    scope().attr("MOOS_DATA") = MOOS_DATA;
    scope().attr("MOOS_POISON") = MOOS_POISON;
    scope().attr("MOOS_WELCOME") = MOOS_WELCOME;
    scope().attr("MOOS_SERVER_REQUEST") = MOOS_SERVER_REQUEST;
    scope().attr("MOOS_SERVER_REQUEST_ID") = MOOS_SERVER_REQUEST_ID;
    scope().attr("MOOS_TIMING") = MOOS_TIMING;
    scope().attr("MOOS_TERMINATE_CONNECTION") = MOOS_TERMINATE_CONNECTION;

    class_<CMOOSMsg>("MOOSMsg", init<>())
        .def(init<char,const std::string &,double,double>())
        .def(init<char ,const std::string &,const std::string &,double>())
        .add_property("type", &CMOOSMsg::GetType,
            "return message type")
        .add_property("time", &CMOOSMsg::GetTime,
            "return time stamp of message")
        .add_property("name", &CMOOSMsg::GetKey,
            "return name of message")
        .add_property("value", &MOOSMsg_GetValue, 
            "return value of message")
        .add_property("source", &CMOOSMsg::GetSource,
            "return the name of the process (as registered with the DB) which posted this notification")
        .add_property("source2", &MOOSMsg_GetSource,
            "return tuple of source and source aux")
        .add_property("community", &CMOOSMsg::GetCommunity,
            "name of the MOOS community in which the orginator lives")
        // twiddly c++ functions if we want them
        .def("isDouble", &CMOOSMsg::IsDouble,
            "check data type is double")
        .def("isString", &CMOOSMsg::IsString,
            "check data type is string")
        .def("isBinary", &CMOOSMsg::IsBinary,
            "check if data type is binary")
        .def("getBinaryData", &MOOSMsg_GetBinaryData,
            "simply access binary data, return None if message is not binary")
        .def("getDouble", &CMOOSMsg::GetDouble,
            "return double val of message")
        .def("getString", &CMOOSMsg::GetString,
            "return string value of message")
        .def("getAsString", &CMOOSMsg::GetAsString, 
                CMOOSMsg__GetAsString(
                    args("fieldwidth", "numDP"),
                    "format the message as string regardless of type"))
        .def("__str__", &MOOSMsg_str)
        ;

    class_<MOOSMSG_LIST> r = export_STLList<CMOOSMsg, Msg_equals>("MOOSMSG_LIST");
    r.def("PeekMail", &MOOSMSG_LIST_PeekMail, 
            MOOSMSG_LIST__PeekMail(args("mail","key","erase","find_youngest"), 
            "helper function that lets a user browse a mail list \n"
            "the message is removed if erase is true"));

    class_<Poco::Exception> sysxpt("Poco::Exception", no_init);

    Poco_Exception_Type = sysxpt.ptr();
    register_exception_translator<Poco::Exception>(&SpitYeBloodyMessageOutAlready);



    class_<CMOOSSerialPort, CMOOSSerialPort*, boost::noncopyable>("MOOSSerialPort",no_init)
        .add_property("name", &CMOOSSerialPort::GetPortName)
        .def("close", &CMOOSSerialPort::Close)
        .add_property("baudrate", &CMOOSSerialPort::GetBaudRate)
        .def("ReadNWithTimeOut", &CMOOSSerialPort_ReadNWithTimeOut,
                CMOOSSerialPort__ReadNWithTimeOut(args("buflen","timeout")))
        .def("Write", &CMOOSSerialPort_Write)
        .def("GetTelegram", &CMOOSSerialPort_GetTelegram);

    def("MOOSTime", &MOOSTime, 
    MOOSTime_Overload(args("applyTimeWarpings"),
        "return time as a double (time since unix in seconds). This will\n"
        "also apply a skew to this time so that all processes connected to\n"
        "a MOOSCommsServer (often in the shape of a DB) will have a unified\n"
        "time. Of course if your process isn't using MOOSComms at all this\n"
        "funtion works just fine and returns the unadulterated time as you\n"
        "would expect"));
          
}
