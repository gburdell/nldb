//Hook to nldb.jar via jni

#include <iostream>
#include <string>
#include <jni.h>
#include "xyzzy/assert.hxx"

using namespace xyzzy;
using namespace std;

static
jmethodID getStaticMethodId(JNIEnv *env, string clsNm, string methodNm, string sig) {
    jclass cls;
	cls = env->FindClass(clsNm.c_str());
	ASSERT_TRUE(NULL != cls);
	jmethodID mid = env->GetStaticMethodID(cls, methodNm.c_str(), 
	                             sig.c_str());
	ASSERT_TRUE(NULL != mid);
	return mid;
}

static 
void checkForException(JNIEnv *env) {
	if (JNI_FALSE != env->ExceptionCheck()) {
		env->ExceptionDescribe();
	}
}

static
string getString(JNIEnv *env, jstring s) {
	const char *p = env->GetStringUTFChars(s, NULL);
	ASSERT_TRUE(NULL != p);
	string str = p;
	env->ReleaseStringUTFChars(s, p);
	return str;
}

int main(int argc, char *argv[]) {
	ASSERT_TRUE(3 == argc);
	JNIEnv *env;
    JavaVM *jvm;
    jint res;
    jclass cls;
    jstring jstr;
    jclass stringClass;
    jobjectArray args;
    JavaVMInitArgs vm_args;
    vm_args.nOptions = 2;
    JavaVMOption options[vm_args.nOptions];
	//use link for now to nldb.jar
    options[0].optionString = "-Djava.class.path=./nldb.jar";
    options[1].optionString = "-Xmx16000m";
    vm_args.version = JNI_VERSION_1_6;
    vm_args.options = options;
    vm_args.ignoreUnrecognized = JNI_TRUE;
    /* Create the Java VM */
    res = JNI_CreateJavaVM(&jvm, (void**)&env, &vm_args);
	ASSERT_TRUE(0 == res);

	string clz = "nldb/jni/Module";
	jstring mod = env->NewStringUTF(argv[1]);
	jstring fnm = env->NewStringUTF(argv[2]);
    jmethodID mid[4];
	mid[0] = getStaticMethodId(env, clz, "load",
		"(Ljava/lang/String;Ljava/lang/String;)I");
	mid[1] = getStaticMethodId(env, clz, "getCells", "(I)I");
	mid[2] = getStaticMethodId(env, clz, "hasNext", "(I)Z");
	mid[3] = getStaticMethodId(env, clz, "nextAsString", "(I)Ljava/lang/String;");
	jint modId = env->CallStaticIntMethod(cls, mid[0], mod, fnm);
	checkForException(env);
	jint cellsId = env->CallStaticIntMethod(cls, mid[1], modId);
	checkForException(env);
	int cnt = 0;
	while (true) {
		jboolean hasNext = env->CallStaticIntMethod(cls, mid[2], cellsId);
		checkForException(env);
		if (hasNext != JNI_TRUE) break;	//while
		jstring jnm = (jstring)env->CallStaticObjectMethod(cls, mid[3], cellsId); 
		checkForException(env);
		string nm = getString(env, jnm);
		cout << "name=" << nm << endl;
		cnt++;
	}
	cout << "cnt=" << cnt << endl;
/*
In C++, however, you need to insert an explicit conversion:
jstring jstr = (jstring)env->GetObjectArrayElement(arr, i);
because jstring is a subtype of jobject, the return type of GetObjectArrayElement.
*/
	jvm->DestroyJavaVM();
	return 0;
}
