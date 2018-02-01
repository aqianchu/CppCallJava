// JvmExample.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
//new.cpp
//
#include "windows.h"
#include "jni.h"
#include <string>
#include <iostream>
#include <windows.h>

using namespace std;

#ifdef UNICODE
#define LoadLibrary  LoadLibraryW
#else
#define LoadLibrary  LoadLibraryA
#endif // !UNICODE

//string 转wchar_t*
wchar_t *multiByteToWideChar(const string& pKey)
{
	const char* pCStrKey = pKey.c_str();
	//第一次调用返回转换后的字符串长度，用于确认为wchar_t*开辟多大的内存空间
	int pSize = MultiByteToWideChar(CP_OEMCP, 0, pCStrKey, strlen(pCStrKey) + 1, NULL, 0);
	wchar_t *pWCStrKey = new wchar_t[pSize];
	//第二次调用将单字节字符串转换成双字节字符串
	MultiByteToWideChar(CP_OEMCP, 0, pCStrKey, strlen(pCStrKey) + 1, pWCStrKey, pSize);
	return pWCStrKey;
}

//wchar_t* 转char*
char* wideCharToMultiByte(wchar_t* pWCStrKey)
{
	//第一次调用确认转换后单字节字符串的长度，用于开辟空间
	int pSize = WideCharToMultiByte(CP_OEMCP, 0, pWCStrKey, wcslen(pWCStrKey), NULL, 0, NULL, NULL);
	char* pCStrKey = new char[pSize + 1];
	//第二次调用将双字节字符串转换成单字节字符串
	WideCharToMultiByte(CP_OEMCP, 0, pWCStrKey, wcslen(pWCStrKey), pCStrKey, pSize, NULL, NULL);
	pCStrKey[pSize] = '\0';
	return pCStrKey;

	//如果想要转换成string，直接赋值即可
	//string pKey = pCStrKey;
}
//wstring转string
std::string wstringToString(const std::wstring& wstr)
{
	LPCWSTR pwszSrc = wstr.c_str();
	int nLen = WideCharToMultiByte(CP_ACP, 0, pwszSrc, -1, NULL, 0, NULL, NULL);
	if (nLen == 0)
		return std::string("");

	char* pszDst = new char[nLen];
	if (!pszDst)
		return std::string("");

	WideCharToMultiByte(CP_ACP, 0, pwszSrc, -1, pszDst, nLen, NULL, NULL);
	std::string str(pszDst);
	delete[] pszDst;
	pszDst = NULL;

	return str;
}

int _tmain(int argc, _TCHAR* argv[])
{
	int res;                      //虚拟机标示
	JavaVMInitArgs vm_args;       //表示可以用来初始化 JVM 的各种 JVM 参数。
	JavaVMOption options[3];      //JavaVMOption options[] 具有用于 JVM 的各种选项设置
	JavaVM *jvm;                  // 是指向 JVM 的指针,我们主要使用这个指针来创建、初始化和销毁 JVM
	JNIEnv *env;                  //表示 JNI 执行环境
	jmethodID mid;                //类中方法的ID                

	options[0].optionString = "-Djava.compiler=NONE";

	//设置classpath，如果程序用到了第三方的JAR包，也可以在这里面包含进来
	options[1].optionString = "-Djava.class.path=.";

	//设置显示消息的类型，取值有gc、class和jni，如果一次取多个的话值之间用逗号格开，如-verbose:gc,class  
	//该参数可以用来观察C++调用JAVA的过程，设置该参数后，程序会在标准输出设备上打印调用的相关信息  
	options[2].optionString = "-verbose:NONE";

	vm_args.version = JNI_VERSION_1_8;     //设置版本号
	vm_args.nOptions = 3;
	vm_args.options = options;
	//该参数指定是否忽略非标准的参数，如果填JNI_FLASE，当遇到非标准参数时，JNI_CreateJavaVM会返回JNI_ERR 
	vm_args.ignoreUnrecognized = JNI_TRUE;

	//定义一个函数指针，下面用来指向JVM中的JNI_CreateJavaVM函数
	typedef jint(WINAPI *PFunCreateJavaVM)(JavaVM **, void **, void *);
	const wchar_t szJvmPath[] = L"C:\\Program Files\\Java\\jdk1.8.0_91\\jre\\bin\\server\\jvm.dll";

	//打印地址
	//wstring tempszJ(szJvmPath);
	//cout << "jvm.dll path:" << wstringToString(tempszJ) << endl;

	//这里需要确保include和lib都包含进来了。加载JVM.DLL动态库
	HINSTANCE hInstance = ::LoadLibrary(szJvmPath);

	//这里我加载失败过，126的错误:在属性/链接器/输入/附加依赖项中添加jvm.lib；
	//193的错误可能是因为你当前编译的平台和加载jvm.dll不是同一个位数的，如果dll是64位则将当前工程改为x64即可。
	if (hInstance == NULL)
	{
		cout << "LoadLibrary False" << endl;
		cout << ::GetLastError() << endl;
		return -2;
	}
	//取得里面的JNI_CreateJavaVM函数指针
	PFunCreateJavaVM funCreateJavaVM = (PFunCreateJavaVM)::GetProcAddress(hInstance, "JNI_CreateJavaVM");
	//调用JNI_CreateJavaVM创建虚拟机
	res = (*funCreateJavaVM)(&jvm, (void**)&env, &vm_args);
	if (res < 0)
	{
		return -1;
	}
	//查找类，返回JAVA类的CLASS对象
	jclass cls = env->FindClass("Tianyu");
	//根据类的CLASS对象获取该类的实例  
	jobject obj = env->AllocObject(cls);
	if (cls == 0){
		cout << "can't find the class\n";
		return -2;
	}
	if (cls != 0)
	{
		//获取类中的方法，最后一个参数是方法的签名，通过javap -s -p [\path\filename.class] 获得
		mid = env->GetStaticMethodID(cls, "getInt", "(I)I");
		if (mid != 0)
		{
			//构造参数并调用对象的方法
			jint square = env->CallStaticIntMethod(cls, mid, 8);
			cout << "Result of intMethod  :" << square << endl;
		}
		
		jclass stuCls = env->FindClass("Tianyu$Student");
		jobject stu = env->AllocObject(stuCls);

		if (!stuCls) {
			cout << "load class fail";
			return -2;
		}
		else {
			//给mid重新赋值为printStudent函数
			//利用javap -s -p Tianyu.class生成函数的签名信息，其中-s表示打印签名信息 
			//-p表示打印所有函数和成员的签名信息，默认只打印public的签名信息。
			mid = env->GetStaticMethodID(cls, "printStudent", "(LTianyu$Student;)V");
			if (mid != 0) {
				env->CallStaticVoidMethod(cls, mid, stu);
			}
		}
	}
	//销毁虚拟机并释放动态库
	jvm->DestroyJavaVM();
	::FreeLibrary(hInstance);

	return 0;
}
