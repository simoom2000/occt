/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class CASCADESamplesJni_V2d_0005fView */

#ifndef _Included_CASCADESamplesJni_V2d_0005fView
#define _Included_CASCADESamplesJni_V2d_0005fView
#ifdef __cplusplus
extern "C" {
#endif
/* Inaccessible static: myCasLock */
/*
 * Class:     CASCADESamplesJni_V2d_0005fView
 * Method:    Center
 * Signature: (Ljcas/Standard_Real;Ljcas/Standard_Real;)V
 */
JNIEXPORT void JNICALL Java_CASCADESamplesJni_V2d_1View_Center
  (JNIEnv *, jobject, jobject, jobject);

/*
 * Class:     CASCADESamplesJni_V2d_0005fView
 * Method:    DefaultHighlightColor
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_CASCADESamplesJni_V2d_1View_DefaultHighlightColor
  (JNIEnv *, jobject);

/*
 * Class:     CASCADESamplesJni_V2d_0005fView
 * Method:    Deflection
 * Signature: ()D
 */
JNIEXPORT jdouble JNICALL Java_CASCADESamplesJni_V2d_1View_Deflection
  (JNIEnv *, jobject);

/*
 * Class:     CASCADESamplesJni_V2d_0005fView
 * Method:    DisableStorePrevious
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_CASCADESamplesJni_V2d_1View_DisableStorePrevious
  (JNIEnv *, jobject);

/*
 * Class:     CASCADESamplesJni_V2d_0005fView
 * Method:    Driver
 * Signature: ()LCASCADESamplesJni/Aspect_WindowDriver;
 */
JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_V2d_1View_Driver
  (JNIEnv *, jobject);

/*
 * Class:     CASCADESamplesJni_V2d_0005fView
 * Method:    EnableStorePrevious
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_CASCADESamplesJni_V2d_1View_EnableStorePrevious
  (JNIEnv *, jobject);

/*
 * Class:     CASCADESamplesJni_V2d_0005fView
 * Method:    Erase
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_CASCADESamplesJni_V2d_1View_Erase
  (JNIEnv *, jobject);

/*
 * Class:     CASCADESamplesJni_V2d_0005fView
 * Method:    EraseHit
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_CASCADESamplesJni_V2d_1View_EraseHit
  (JNIEnv *, jobject);

/*
 * Class:     CASCADESamplesJni_V2d_0005fView
 * Method:    Fitall
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_CASCADESamplesJni_V2d_1View_Fitall
  (JNIEnv *, jobject);

/*
 * Class:     CASCADESamplesJni_V2d_0005fView
 * Method:    HasBeenMoved
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_CASCADESamplesJni_V2d_1View_HasBeenMoved
  (JNIEnv *, jobject);

/*
 * Class:     CASCADESamplesJni_V2d_0005fView
 * Method:    Hit
 * Signature: (IILjcas/Standard_Real;Ljcas/Standard_Real;)V
 */
JNIEXPORT void JNICALL Java_CASCADESamplesJni_V2d_1View_Hit
  (JNIEnv *, jobject, jint, jint, jobject, jobject);

/*
 * Class:     CASCADESamplesJni_V2d_0005fView
 * Method:    Magnify
 * Signature: (LCASCADESamplesJni/V2d_View;IIII)V
 */
JNIEXPORT void JNICALL Java_CASCADESamplesJni_V2d_1View_Magnify
  (JNIEnv *, jobject, jobject, jint, jint, jint, jint);

/*
 * Class:     CASCADESamplesJni_V2d_0005fView
 * Method:    MustBeResized
 * Signature: (S)V
 */
JNIEXPORT void JNICALL Java_CASCADESamplesJni_V2d_1View_MustBeResized
  (JNIEnv *, jobject, jshort);

/*
 * Class:     CASCADESamplesJni_V2d_0005fView
 * Method:    Pan
 * Signature: (II)V
 */
JNIEXPORT void JNICALL Java_CASCADESamplesJni_V2d_1View_Pan
  (JNIEnv *, jobject, jint, jint);

/*
 * Class:     CASCADESamplesJni_V2d_0005fView
 * Method:    PickByCircle
 * Signature: (III)LCASCADESamplesJni/Graphic2d_DisplayList;
 */
JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_V2d_1View_PickByCircle
  (JNIEnv *, jobject, jint, jint, jint);

/*
 * Class:     CASCADESamplesJni_V2d_0005fView
 * Method:    Place
 * Signature: (IID)V
 */
JNIEXPORT void JNICALL Java_CASCADESamplesJni_V2d_1View_Place
  (JNIEnv *, jobject, jint, jint, jdouble);

/*
 * Class:     CASCADESamplesJni_V2d_0005fView
 * Method:    PlotScreen
 * Signature: (LCASCADESamplesJni/PlotMgt_PlotterDriver;)V
 */
JNIEXPORT void JNICALL Java_CASCADESamplesJni_V2d_1View_PlotScreen
  (JNIEnv *, jobject, jobject);

/*
 * Class:     CASCADESamplesJni_V2d_0005fView
 * Method:    PostScriptOutput
 * Signature: (Ljcas/Standard_CString;DDDDDS)V
 */
JNIEXPORT void JNICALL Java_CASCADESamplesJni_V2d_1View_PostScriptOutput
  (JNIEnv *, jobject, jobject, jdouble, jdouble, jdouble, jdouble, jdouble, jshort);

/*
 * Class:     CASCADESamplesJni_V2d_0005fView
 * Method:    Previous
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_CASCADESamplesJni_V2d_1View_Previous
  (JNIEnv *, jobject);

/*
 * Class:     CASCADESamplesJni_V2d_0005fView
 * Method:    Reset
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_CASCADESamplesJni_V2d_1View_Reset
  (JNIEnv *, jobject);

/*
 * Class:     CASCADESamplesJni_V2d_0005fView
 * Method:    Restore
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_CASCADESamplesJni_V2d_1View_Restore
  (JNIEnv *, jobject);

/*
 * Class:     CASCADESamplesJni_V2d_0005fView
 * Method:    RestoreArea
 * Signature: (IIII)V
 */
JNIEXPORT void JNICALL Java_CASCADESamplesJni_V2d_1View_RestoreArea
  (JNIEnv *, jobject, jint, jint, jint, jint);

/*
 * Class:     CASCADESamplesJni_V2d_0005fView
 * Method:    ScreenCopy
 * Signature: (LCASCADESamplesJni/PlotMgt_PlotterDriver;ZD)V
 */
JNIEXPORT void JNICALL Java_CASCADESamplesJni_V2d_1View_ScreenCopy
  (JNIEnv *, jobject, jobject, jboolean, jdouble);

/*
 * Class:     CASCADESamplesJni_V2d_0005fView
 * Method:    ScreenPlace
 * Signature: (DDD)V
 */
JNIEXPORT void JNICALL Java_CASCADESamplesJni_V2d_1View_ScreenPlace
  (JNIEnv *, jobject, jdouble, jdouble, jdouble);

/*
 * Class:     CASCADESamplesJni_V2d_0005fView
 * Method:    ScreenPostScriptOutput
 * Signature: (Ljcas/Standard_CString;DDS)V
 */
JNIEXPORT void JNICALL Java_CASCADESamplesJni_V2d_1View_ScreenPostScriptOutput
  (JNIEnv *, jobject, jobject, jdouble, jdouble, jshort);

/*
 * Class:     CASCADESamplesJni_V2d_0005fView
 * Method:    Scroll
 * Signature: (Ljcas/Standard_Integer;Ljcas/Standard_Integer;Ljcas/Standard_Integer;Ljcas/Standard_Integer;)V
 */
JNIEXPORT void JNICALL Java_CASCADESamplesJni_V2d_1View_Scroll
  (JNIEnv *, jobject, jobject, jobject, jobject, jobject);

/*
 * Class:     CASCADESamplesJni_V2d_0005fView
 * Method:    SetDefaultHighlightColor
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_CASCADESamplesJni_V2d_1View_SetDefaultHighlightColor
  (JNIEnv *, jobject, jint);

/*
 * Class:     CASCADESamplesJni_V2d_0005fView
 * Method:    SetDefaultPosition
 * Signature: (DDD)V
 */
JNIEXPORT void JNICALL Java_CASCADESamplesJni_V2d_1View_SetDefaultPosition
  (JNIEnv *, jobject, jdouble, jdouble, jdouble);

/*
 * Class:     CASCADESamplesJni_V2d_0005fView
 * Method:    SetDeflection
 * Signature: (D)V
 */
JNIEXPORT void JNICALL Java_CASCADESamplesJni_V2d_1View_SetDeflection
  (JNIEnv *, jobject, jdouble);

/*
 * Class:     CASCADESamplesJni_V2d_0005fView
 * Method:    SetFitallRatio
 * Signature: (D)V
 */
JNIEXPORT void JNICALL Java_CASCADESamplesJni_V2d_1View_SetFitallRatio
  (JNIEnv *, jobject, jdouble);

/*
 * Class:     CASCADESamplesJni_V2d_0005fView
 * Method:    ShowHit
 * Signature: (II)V
 */
JNIEXPORT void JNICALL Java_CASCADESamplesJni_V2d_1View_ShowHit
  (JNIEnv *, jobject, jint, jint);

/*
 * Class:     CASCADESamplesJni_V2d_0005fView
 * Method:    Size
 * Signature: ()D
 */
JNIEXPORT jdouble JNICALL Java_CASCADESamplesJni_V2d_1View_Size
  (JNIEnv *, jobject);

/*
 * Class:     CASCADESamplesJni_V2d_0005fView
 * Method:    Translate
 * Signature: (DD)V
 */
JNIEXPORT void JNICALL Java_CASCADESamplesJni_V2d_1View_Translate
  (JNIEnv *, jobject, jdouble, jdouble);

/*
 * Class:     CASCADESamplesJni_V2d_0005fView
 * Method:    Update
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_CASCADESamplesJni_V2d_1View_Update
  (JNIEnv *, jobject);

/*
 * Class:     CASCADESamplesJni_V2d_0005fView
 * Method:    UpdateNew
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_CASCADESamplesJni_V2d_1View_UpdateNew
  (JNIEnv *, jobject);

/*
 * Class:     CASCADESamplesJni_V2d_0005fView
 * Method:    V2d_View_Color_1
 * Signature: ()S
 */
JNIEXPORT jshort JNICALL Java_CASCADESamplesJni_V2d_1View_V2d_1View_1Color_11
  (JNIEnv *, jobject);

/*
 * Class:     CASCADESamplesJni_V2d_0005fView
 * Method:    V2d_View_Color_2
 * Signature: (LCASCADESamplesJni/Quantity_Color;)V
 */
JNIEXPORT void JNICALL Java_CASCADESamplesJni_V2d_1View_V2d_1View_1Color_12
  (JNIEnv *, jobject, jobject);

/*
 * Class:     CASCADESamplesJni_V2d_0005fView
 * Method:    V2d_View_Convert_1
 * Signature: (I)D
 */
JNIEXPORT jdouble JNICALL Java_CASCADESamplesJni_V2d_1View_V2d_1View_1Convert_11
  (JNIEnv *, jobject, jint);

/*
 * Class:     CASCADESamplesJni_V2d_0005fView
 * Method:    V2d_View_Convert_2
 * Signature: (IILjcas/Standard_Real;Ljcas/Standard_Real;)V
 */
JNIEXPORT void JNICALL Java_CASCADESamplesJni_V2d_1View_V2d_1View_1Convert_12
  (JNIEnv *, jobject, jint, jint, jobject, jobject);

/*
 * Class:     CASCADESamplesJni_V2d_0005fView
 * Method:    V2d_View_Convert_3
 * Signature: (DDLjcas/Standard_Integer;Ljcas/Standard_Integer;)V
 */
JNIEXPORT void JNICALL Java_CASCADESamplesJni_V2d_1View_V2d_1View_1Convert_13
  (JNIEnv *, jobject, jdouble, jdouble, jobject, jobject);

/*
 * Class:     CASCADESamplesJni_V2d_0005fView
 * Method:    V2d_View_Convert_4
 * Signature: (D)D
 */
JNIEXPORT jdouble JNICALL Java_CASCADESamplesJni_V2d_1View_V2d_1View_1Convert_14
  (JNIEnv *, jobject, jdouble);

/*
 * Class:     CASCADESamplesJni_V2d_0005fView
 * Method:    V2d_View_Create_0
 * Signature: (LCASCADESamplesJni/Aspect_WindowDriver;LCASCADESamplesJni/V2d_Viewer;DDD)V
 */
JNIEXPORT void JNICALL Java_CASCADESamplesJni_V2d_1View_V2d_1View_1Create_10
  (JNIEnv *, jobject, jobject, jobject, jdouble, jdouble, jdouble);

/*
 * Class:     CASCADESamplesJni_V2d_0005fView
 * Method:    V2d_View_Dump_1
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_CASCADESamplesJni_V2d_1View_V2d_1View_1Dump_11
  (JNIEnv *, jobject);

/*
 * Class:     CASCADESamplesJni_V2d_0005fView
 * Method:    V2d_View_Dump_2
 * Signature: (Ljcas/Standard_CString;)V
 */
JNIEXPORT void JNICALL Java_CASCADESamplesJni_V2d_1View_V2d_1View_1Dump_12
  (JNIEnv *, jobject, jobject);

/*
 * Class:     CASCADESamplesJni_V2d_0005fView
 * Method:    V2d_View_Fit_1
 * Signature: (DDDDZ)V
 */
JNIEXPORT void JNICALL Java_CASCADESamplesJni_V2d_1View_V2d_1View_1Fit_11
  (JNIEnv *, jobject, jdouble, jdouble, jdouble, jdouble, jboolean);

/*
 * Class:     CASCADESamplesJni_V2d_0005fView
 * Method:    V2d_View_Fit_2
 * Signature: (IIII)V
 */
JNIEXPORT void JNICALL Java_CASCADESamplesJni_V2d_1View_V2d_1View_1Fit_12
  (JNIEnv *, jobject, jint, jint, jint, jint);

/*
 * Class:     CASCADESamplesJni_V2d_0005fView
 * Method:    V2d_View_Pick_1
 * Signature: (III)LCASCADESamplesJni/Graphic2d_DisplayList;
 */
JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_V2d_1View_V2d_1View_1Pick_11
  (JNIEnv *, jobject, jint, jint, jint);

/*
 * Class:     CASCADESamplesJni_V2d_0005fView
 * Method:    V2d_View_Pick_2
 * Signature: (IIIIS)LCASCADESamplesJni/Graphic2d_DisplayList;
 */
JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_V2d_1View_V2d_1View_1Pick_12
  (JNIEnv *, jobject, jint, jint, jint, jint, jshort);

/*
 * Class:     CASCADESamplesJni_V2d_0005fView
 * Method:    V2d_View_Plot_1
 * Signature: (LCASCADESamplesJni/PlotMgt_PlotterDriver;DDD)V
 */
JNIEXPORT void JNICALL Java_CASCADESamplesJni_V2d_1View_V2d_1View_1Plot_11
  (JNIEnv *, jobject, jobject, jdouble, jdouble, jdouble);

/*
 * Class:     CASCADESamplesJni_V2d_0005fView
 * Method:    V2d_View_Plot_2
 * Signature: (LCASCADESamplesJni/PlotMgt_PlotterDriver;D)V
 */
JNIEXPORT void JNICALL Java_CASCADESamplesJni_V2d_1View_V2d_1View_1Plot_12
  (JNIEnv *, jobject, jobject, jdouble);

/*
 * Class:     CASCADESamplesJni_V2d_0005fView
 * Method:    V2d_View_SetBackground_1
 * Signature: (S)V
 */
JNIEXPORT void JNICALL Java_CASCADESamplesJni_V2d_1View_V2d_1View_1SetBackground_11
  (JNIEnv *, jobject, jshort);

/*
 * Class:     CASCADESamplesJni_V2d_0005fView
 * Method:    V2d_View_SetBackground_2
 * Signature: (LCASCADESamplesJni/Quantity_Color;)V
 */
JNIEXPORT void JNICALL Java_CASCADESamplesJni_V2d_1View_V2d_1View_1SetBackground_12
  (JNIEnv *, jobject, jobject);

/*
 * Class:     CASCADESamplesJni_V2d_0005fView
 * Method:    V2d_View_SetBackground_3
 * Signature: (Ljcas/Standard_CString;S)Z
 */
JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_V2d_1View_V2d_1View_1SetBackground_13
  (JNIEnv *, jobject, jobject, jshort);

/*
 * Class:     CASCADESamplesJni_V2d_0005fView
 * Method:    V2d_View_Zoom_1
 * Signature: (D)V
 */
JNIEXPORT void JNICALL Java_CASCADESamplesJni_V2d_1View_V2d_1View_1Zoom_11
  (JNIEnv *, jobject, jdouble);

/*
 * Class:     CASCADESamplesJni_V2d_0005fView
 * Method:    V2d_View_Zoom_2
 * Signature: (IIIID)V
 */
JNIEXPORT void JNICALL Java_CASCADESamplesJni_V2d_1View_V2d_1View_1Zoom_12
  (JNIEnv *, jobject, jint, jint, jint, jint, jdouble);

/*
 * Class:     CASCADESamplesJni_V2d_0005fView
 * Method:    V2d_View_Zoom_3
 * Signature: (IID)V
 */
JNIEXPORT void JNICALL Java_CASCADESamplesJni_V2d_1View_V2d_1View_1Zoom_13
  (JNIEnv *, jobject, jint, jint, jdouble);

/*
 * Class:     CASCADESamplesJni_V2d_0005fView
 * Method:    V2d_View_Zoom_4
 * Signature: ()D
 */
JNIEXPORT jdouble JNICALL Java_CASCADESamplesJni_V2d_1View_V2d_1View_1Zoom_14
  (JNIEnv *, jobject);

/*
 * Class:     CASCADESamplesJni_V2d_0005fView
 * Method:    View
 * Signature: ()LCASCADESamplesJni/Graphic2d_View;
 */
JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_V2d_1View_View
  (JNIEnv *, jobject);

/*
 * Class:     CASCADESamplesJni_V2d_0005fView
 * Method:    Viewer
 * Signature: ()LCASCADESamplesJni/V2d_Viewer;
 */
JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_V2d_1View_Viewer
  (JNIEnv *, jobject);

/*
 * Class:     CASCADESamplesJni_V2d_0005fView
 * Method:    WindowFit
 * Signature: (IIII)V
 */
JNIEXPORT void JNICALL Java_CASCADESamplesJni_V2d_1View_WindowFit
  (JNIEnv *, jobject, jint, jint, jint, jint);

#ifdef __cplusplus
}
#endif
#endif
