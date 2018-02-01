// JvmExample.cpp : �������̨Ӧ�ó������ڵ㡣
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

//string תwchar_t*
wchar_t *multiByteToWideChar(const string& pKey)
{
	const char* pCStrKey = pKey.c_str();
	//��һ�ε��÷���ת������ַ������ȣ�����ȷ��Ϊwchar_t*���ٶ����ڴ�ռ�
	int pSize = MultiByteToWideChar(CP_OEMCP, 0, pCStrKey, strlen(pCStrKey) + 1, NULL, 0);
	wchar_t *pWCStrKey = new wchar_t[pSize];
	//�ڶ��ε��ý����ֽ��ַ���ת����˫�ֽ��ַ���
	MultiByteToWideChar(CP_OEMCP, 0, pCStrKey, strlen(pCStrKey) + 1, pWCStrKey, pSize);
	return pWCStrKey;
}

//wchar_t* תchar*
char* wideCharToMultiByte(wchar_t* pWCStrKey)
{
	//��һ�ε���ȷ��ת�����ֽ��ַ����ĳ��ȣ����ڿ��ٿռ�
	int pSize = WideCharToMultiByte(CP_OEMCP, 0, pWCStrKey, wcslen(pWCStrKey), NULL, 0, NULL, NULL);
	char* pCStrKey = new char[pSize + 1];
	//�ڶ��ε��ý�˫�ֽ��ַ���ת���ɵ��ֽ��ַ���
	WideCharToMultiByte(CP_OEMCP, 0, pWCStrKey, wcslen(pWCStrKey), pCStrKey, pSize, NULL, NULL);
	pCStrKey[pSize] = '\0';
	return pCStrKey;

	//�����Ҫת����string��ֱ�Ӹ�ֵ����
	//string pKey = pCStrKey;
}
//wstringתstring
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
	int res;                      //�������ʾ
	JavaVMInitArgs vm_args;       //��ʾ����������ʼ�� JVM �ĸ��� JVM ������
	JavaVMOption options[3];      //JavaVMOption options[] �������� JVM �ĸ���ѡ������
	JavaVM *jvm;                  // ��ָ�� JVM ��ָ��,������Ҫʹ�����ָ������������ʼ�������� JVM
	JNIEnv *env;                  //��ʾ JNI ִ�л���
	jmethodID mid;                //���з�����ID                

	options[0].optionString = "-Djava.compiler=NONE";

	//����classpath����������õ��˵�������JAR����Ҳ�������������������
	options[1].optionString = "-Djava.class.path=.";

	//������ʾ��Ϣ�����ͣ�ȡֵ��gc��class��jni�����һ��ȡ����Ļ�ֵ֮���ö��Ÿ񿪣���-verbose:gc,class  
	//�ò������������۲�C++����JAVA�Ĺ��̣����øò����󣬳�����ڱ�׼����豸�ϴ�ӡ���õ������Ϣ  
	options[2].optionString = "-verbose:NONE";

	vm_args.version = JNI_VERSION_1_8;     //���ð汾��
	vm_args.nOptions = 3;
	vm_args.options = options;
	//�ò���ָ���Ƿ���ԷǱ�׼�Ĳ����������JNI_FLASE���������Ǳ�׼����ʱ��JNI_CreateJavaVM�᷵��JNI_ERR 
	vm_args.ignoreUnrecognized = JNI_TRUE;

	//����һ������ָ�룬��������ָ��JVM�е�JNI_CreateJavaVM����
	typedef jint(WINAPI *PFunCreateJavaVM)(JavaVM **, void **, void *);
	const wchar_t szJvmPath[] = L"C:\\Program Files\\Java\\jdk1.8.0_91\\jre\\bin\\server\\jvm.dll";

	//��ӡ��ַ
	//wstring tempszJ(szJvmPath);
	//cout << "jvm.dll path:" << wstringToString(tempszJ) << endl;

	//������Ҫȷ��include��lib�����������ˡ�����JVM.DLL��̬��
	HINSTANCE hInstance = ::LoadLibrary(szJvmPath);

	//�����Ҽ���ʧ�ܹ���126�Ĵ���:������/������/����/���������������jvm.lib��
	//193�Ĵ����������Ϊ�㵱ǰ�����ƽ̨�ͼ���jvm.dll����ͬһ��λ���ģ����dll��64λ�򽫵�ǰ���̸�Ϊx64���ɡ�
	if (hInstance == NULL)
	{
		cout << "LoadLibrary False" << endl;
		cout << ::GetLastError() << endl;
		return -2;
	}
	//ȡ�������JNI_CreateJavaVM����ָ��
	PFunCreateJavaVM funCreateJavaVM = (PFunCreateJavaVM)::GetProcAddress(hInstance, "JNI_CreateJavaVM");
	//����JNI_CreateJavaVM���������
	res = (*funCreateJavaVM)(&jvm, (void**)&env, &vm_args);
	if (res < 0)
	{
		return -1;
	}
	//�����࣬����JAVA���CLASS����
	jclass cls = env->FindClass("Tianyu");
	//�������CLASS�����ȡ�����ʵ��  
	jobject obj = env->AllocObject(cls);
	if (cls == 0){
		cout << "can't find the class\n";
		return -2;
	}
	if (cls != 0)
	{
		//��ȡ���еķ��������һ�������Ƿ�����ǩ����ͨ��javap -s -p [\path\filename.class] ���
		mid = env->GetStaticMethodID(cls, "getInt", "(I)I");
		if (mid != 0)
		{
			//������������ö���ķ���
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
			//��mid���¸�ֵΪprintStudent����
			//����javap -s -p Tianyu.class���ɺ�����ǩ����Ϣ������-s��ʾ��ӡǩ����Ϣ 
			//-p��ʾ��ӡ���к����ͳ�Ա��ǩ����Ϣ��Ĭ��ֻ��ӡpublic��ǩ����Ϣ��
			mid = env->GetStaticMethodID(cls, "printStudent", "(LTianyu$Student;)V");
			if (mid != 0) {
				env->CallStaticVoidMethod(cls, mid, stu);
			}
		}
	}
	//������������ͷŶ�̬��
	jvm->DestroyJavaVM();
	::FreeLibrary(hInstance);

	return 0;
}
