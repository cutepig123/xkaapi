
template<typename T>
struct TaskDOT<TaskGEMM<T> > {
    static const char* name() { return "GEMM"; }
    static const char* color() { return "grey"; }
};

template<typename T>
struct TaskDOT<TaskPOTRF<T> > {
    static const char* name() { return "POTRF"; }
    static const char* color() { return "orange"; }
};

template<typename T>
struct TaskDOT<TaskTRSM<T> > {
    static const char* name() { return "TRSM"; }
    static const char* color() { return "skyblue"; }
};

template<typename T>
struct TaskDOT<TaskSYRK<T> > {
    static const char* name() { return "SYRK"; }
    static const char* color() { return "green"; }
};

template<typename T>
struct TaskDOT<TaskGETRF<T> > {
    static const char* name() { return "GETRF"; }
    static const char* color() { return "orange"; }
};

template<>
struct TaskDOT<TaskPlasmaDGESSM> {
    static const char* name() { return "DGESSM"; }
    static const char* color() { return "orange"; }
};

template<>
struct TaskDOT<TaskPlasmaDTSTRF> {
    static const char* name() { return "DTSTRF"; }
    static const char* color() { return "orange"; }
};

template<>
struct TaskDOT<TaskPlasmaDSSSSM> {
    static const char* name() { return "DSSSSM"; }
    static const char* color() { return "orange"; }
};

