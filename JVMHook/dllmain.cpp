#include <iostream>
#include "dllmain.h"
#include <Windows.h>

JNIEnv* env;
JavaVM* vm;

boolean hasSprint = false;

void jvmHook()
{
    HMODULE jvm = GetModuleHandleA("jvm.dll");
    using t_createdvms = jint(__stdcall*)(JavaVM**, jsize, jsize*);
    FARPROC processAddress = GetProcAddress(reinterpret_cast<HMODULE>(jvm), "JNI_GetCreatedJavaVMs");
    t_createdvms created_java_vms = reinterpret_cast<t_createdvms>(processAddress);
    auto ret = created_java_vms(&vm, 1, nullptr);
    ret = vm->AttachCurrentThread(reinterpret_cast<void**>(&env), nullptr);
    if (ret != JNI_OK) {
        std::cerr << "Failed to attach to JVM" << std::endl;
    }
    else {
        std::cout << "Attached to JVM successfully" << std::endl;
    }
}

void sout(const char* str) {
    jclass class_sys = env->FindClass("java/lang/System");
    jclass class_print_stream = env->FindClass("java/io/PrintStream");
    jfieldID field_id_out = env->GetStaticFieldID(class_sys, "out", "Ljava/io/PrintStream;");
    jobject obj_out = env->GetStaticObjectField(class_sys, field_id_out);
    jmethodID method_id_println = env->GetMethodID(class_print_stream, "println", "(Ljava/lang/String;)V");
    env->CallVoidMethod(obj_out, method_id_println, env->NewStringUTF(str));
}

void initialization() {
    jvmHook();
    sout("JNI included.");
}

BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
) {
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        CreateThread(0, 0, (LPTHREAD_START_ROUTINE)(initialization), 0, 0, 0);
        break;
    }

    return TRUE;
}
