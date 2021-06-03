#include <pthread.h>
#include <jni.h>
#include <memory.h>
#include <dlfcn.h>
#include <cstdio>
#include <cstdlib>
#include <Includes/Vector3.h>
#include <Includes/Vector2.h>
#include <Includes/Quaternion.h>
#include <Includes/Rect.hpp>
#include "Includes/Logger.h"
#include "Patching/Patch.h"
#import "Includes/Utils.h"
#include "Includes/Color.hpp"
#include "Includes/Strings.h"


#if defined(__armv7__)
#include "X64Hook/And64InlineHook.hpp"
#else
#include "Substrate/CydiaSubstrate.h"
#endif

/* 
Big Credits To Chewy And Slice Cast.
This MAY Not Work! This is Just a Test.
*/

bool isChatSpamClan = false;

bool clanchatcontrollerhookinitialized = false;


void (*SendChatMessage)(void* _this, monoString* text, monoString* iconName);

void(*old_ClanChatControllerUpdate)(void *instance);
void ClanChatControllerUpdate(void *instance) {
    if(instance != NULL) {
        if(!ClanChatControllerUpdateHookInitialized){
            clanchatcontrollerhookinitialized = true;
            LOGI("GameManager_LateUpdate hooked");
        }
        if (isChatSpamClan) {
            SendChatMessage(instance, CreateMonoString("Slice Cast And Chewy R Pog"), false, CreateMonoString("Slice Cast And Chewy R Pog"));
        }
    }
    old_ClanChatControllerUpdate(instance);
}

void* hack_thread(void*) {
   LOGI("I have been loaded. Mwuahahahaha");
   // loop until our target library is found
   do {
       sleep(1);
   } while (!isLibraryLoaded(libName));
   LOGI("I found the il2cpp lib. Address is: %p", (void*)findLibrary(libName));
   LOGI("Hooking GameManager_LateUpdate");
   SendChatMessage = (void (*)(void*, monoString*, monoString*))getRealOffset(0x2D5371C); //SendChatMessage:ClanChatController
   octo_hook((void*)getRealOffset(0x2D53F80), (void*)ClanChatControllerUpdate, (void**)&old_ClanChatControllerUpdate);
   return NULL;
}

jobjectArray getListFT(JNIEnv *env, jclass jobj){
   jobjectArray ret;
   int i;
   const char *features[]= {
           "Clan Chat Spam",
   };
   int Total_Feature = (sizeof features / sizeof features[0]); //Now you dont have to manually update the number everytime
   ret= (jobjectArray)env->NewObjectArray(Total_Feature,
                                          env->FindClass("java/lang/String"),
                                          env->NewStringUTF(""));

   for(i=0;i<Total_Feature;i++) {
       env->SetObjectArrayElement(
               ret,i,env->NewStringUTF(features[i]));
   }
   return(ret);
}


void changeToggle(JNIEnv *env, jclass thisObj, jint number) {
   int i = (int) number;
   switch (i) {
       case 0:
	   isChatSpamClan = !isChatSpamClan;
           break;
       default:
           break;
   }
   return;
}

void init(JNIEnv * env, jclass obj, jobject thiz){
   pthread_t ptid;
   pthread_create(&ptid, NULL, hack_thread, NULL);
   MakeToast(env, thiz, "Github Project, Thanks To Chewy And Slice Cast. - https://polarmods.com");
}

void changeSeekBar(JNIEnv *env, jclass clazz, jint i, jint seekbarValue) {
   int li = (int) i;
   switch (li) {
           break;
       default:
           break;
   }
   return;
}

void changeSpinner(JNIEnv *env, jclass clazz, jint i, jstring value) {
   int li = (int) i;
   switch (li) {
       default:
           break;
   }
}

void changeEditText(JNIEnv *env, jclass clazz, jint i, jstring value){
   int li = (int) i;
   switch (li){
       default:
           break;
   }
   return;
}

extern "C"
JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved) {
   JNIEnv* env;
   if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
       return JNI_ERR;
   }

   // Find your class. JNI_OnLoad is called from the correct class loader context for this to work.
   jclass c = env->FindClass("com/dark/force/NativeLibrary");
   if (c == nullptr) return JNI_ERR;

   // Register your class' native methods.
   static const JNINativeMethod methods[] = {
           {"changeEditText", "(ILjava/lang/String;)V", reinterpret_cast<void*>(changeEditText)},
           {"changeSeekBar", "(II)V", reinterpret_cast<void*>(changeSeekBar)},
           {"changeSpinner", "(ILjava/lang/String;)V", reinterpret_cast<void*>(changeSpinner)},
           {"changeToggle", "(I)V", reinterpret_cast<void*>(changeToggle)},
           {"getListFT", "()[Ljava/lang/String;", reinterpret_cast<void*>(getListFT)},
           {"init", "(Lcom/dark/force/MenuService;)V", reinterpret_cast<void*>(init)},
   };
   int rc = env->RegisterNatives(c, methods, sizeof(methods)/sizeof(JNINativeMethod));
   if (rc != JNI_OK) return rc;

   return JNI_VERSION_1_6;
}


