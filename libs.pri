message(libs.pri $$PWD)

win32 {
    INCLUDEPATH += \
        E:\libs\opencv-3.4.5\mingw730_release\install\include \             # opencv 3.4.5
#        E:\libs\opencv-3.4.5\mingw730_release\install\include\opencv \
#        E:\libs\opencv-3.4.5\mingw730_release\install\include\opencv2 \
        E:\libs\dlib\dlib-19.16-mingw730\install\include \                  # dlib 19.17
        E:\libs\flann\flann_1.9.1_mingw730_release\install\include          # flann 1.91

    CONFIG(debug, debug|release) {
        message("I compiled the opencv 3.4.5 debug mode FAILED! USING REALSE MODE")
    }

    CONFIG(release, debug|release) {
        message(RELEASE MODE)

        LIBS += \
#            -LE:\libs\dlib\dlib-19.16-mingw730\install\lib -ldlib \   # dlib 19.17
            -LE:\libs\dlib\dlib-19.16-mingw730_debug\install\lib -ldlib \
            $$PWD/../../libs/opencv-3.4.5/mingw730_release/install/x64/mingw/lib/libopencv_calib3d345.dll.a \   # opencv3.4.5
            $$PWD/../../libs/opencv-3.4.5/mingw730_release/install/x64/mingw/lib/libopencv_core345.dll.a \
            $$PWD/../../libs/opencv-3.4.5/mingw730_release/install/x64/mingw/lib/libopencv_dnn345.dll.a \
            $$PWD/../../libs/opencv-3.4.5/mingw730_release/install/x64/mingw/lib/libopencv_features2d345.dll.a \
            $$PWD/../../libs/opencv-3.4.5/mingw730_release/install/x64/mingw/lib/libopencv_flann345.dll.a \
            $$PWD/../../libs/opencv-3.4.5/mingw730_release/install/x64/mingw/lib/libopencv_highgui345.dll.a \
            $$PWD/../../libs/opencv-3.4.5/mingw730_release/install/x64/mingw/lib/libopencv_imgcodecs345.dll.a \
            $$PWD/../../libs/opencv-3.4.5/mingw730_release/install/x64/mingw/lib/libopencv_imgproc345.dll.a \
            $$PWD/../../libs/opencv-3.4.5/mingw730_release/install/x64/mingw/lib/libopencv_ml345.dll.a \
            $$PWD/../../libs/opencv-3.4.5/mingw730_release/install/x64/mingw/lib/libopencv_objdetect345.dll.a \
            $$PWD/../../libs/opencv-3.4.5/mingw730_release/install/x64/mingw/lib/libopencv_photo345.dll.a \
            $$PWD/../../libs/opencv-3.4.5/mingw730_release/install/x64/mingw/lib/libopencv_shape345.dll.a \
            $$PWD/../../libs/opencv-3.4.5/mingw730_release/install/x64/mingw/lib/libopencv_stitching345.dll.a \
            $$PWD/../../libs/opencv-3.4.5/mingw730_release/install/x64/mingw/lib/libopencv_superres345.dll.a \
            $$PWD/../../libs/opencv-3.4.5/mingw730_release/install/x64/mingw/lib/libopencv_video345.dll.a \
            $$PWD/../../libs/opencv-3.4.5/mingw730_release/install/x64/mingw/lib/libopencv_videoio345.dll.a \
            $$PWD/../../libs/opencv-3.4.5/mingw730_release/install/x64/mingw/lib/libopencv_videostab345.dll.a \
            -L E:\libs\flann\flann_1.9.1_mingw730_release\install\lib -lflann -lflann_cpp                      # flann

    }
}

